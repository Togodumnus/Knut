#ifndef __APPLE__
    #define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "process.h"
#include "server.h"
#include "utils.h"
#include "../DEBUG.h"

const int PIPE_READ  = 0;
const int PIPE_WRITE = 1;

const int BUFFER_SIZE = 1024;

const char EOT = 4;

/**
 * HOSTNAME
 */
const char *HOSTNAME = NULL; //me

/**
 * MAX_QUEUE
 *
 * Nombre max de connexion simultanées au serveur
 */
const int MAX_QUEUE  = 5;

/**
 * getAdresse
 *
 * @return {void *}     Un pointeur vers la structure d'addresse IPV6 ou IPV4
 */
void *getAdresse(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) { //IPV4
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

struct addrinfo *serveur_info;  //full host config returned by getaddrinfo

/**
 * configServeur
 *
 * Configuration du serveur avant lancement d'un socket
 *
 * @param  {char *} port
 */
void configServeur(char *port) {

    struct addrinfo config; //config
    char *hostname = NULL;  //on est serveur

    memset(&config, 0, sizeof(config)); //RAZ de config
    config.ai_family   = AF_UNSPEC;     //IPv4 or IPv6
    config.ai_socktype = SOCK_STREAM;   //TCP
    config.ai_flags    = AI_PASSIVE;    //config d'un serveur

    int result_code;
    if ((result_code = getaddrinfo(hostname, port, &config, &serveur_info)) != 0) {
        fprintf(stderr, "server: %s\n", gai_strerror(result_code));
        exit(EXIT_FAILURE);
    }
}

/**
 * createServerSocket
 *
 * Création d'un socket sur le port `port`
 * Si on atteint une erreur autre que EADDRINUSE, on exit
 *
 * @param  {int}    port
 *
 * @return {int}    -1 si le port est occupé, le socket sinon
 */
SOCKET createServerSocket(int port) {

    char port_buf[6];
    sprintf((char *)port_buf, "%d", port);
    port_buf[5] = '\0';
    configServeur(port_buf);

    //Création du socket serveur
    SOCKET listener;
    listener = socket(
        serveur_info->ai_family,
        serveur_info->ai_socktype,
        serveur_info->ai_protocol
    );

    //Empêcher le blocage du socket si on kill le process
    int yes = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //On tente un bind

    if (bind(listener, serveur_info->ai_addr, serveur_info->ai_addrlen) < 0) {
        close(listener);

        if (errno == EADDRINUSE) { //le port est occupé
            return -1;
        } else {
            exit(EXIT_FAILURE);
        }
    }

    freeaddrinfo(serveur_info); //config terminée

    return listener;
}

/**
 * initSocket
 *
 * Création d'un socket en parcourant tous les ports depuis 1025 à la recherche
 * d'un port disponible
 *
 * @param   {int *}     port    Va être complété avec le port utilisé
 *
 * @return  {SOCKET}    Le socket
 */
SOCKET initSocket(int *port) {
    int p = 1025;
    int listener;
    while ((listener = createServerSocket(p)) == -1) {
        p++;
        close(listener);
        if (p > 65535) {
            break;
        }
    }

    if (listener >= 0) {
        *port = p;
        if (listen(listener, MAX_QUEUE) == -1) {
            perror("listen()");
            exit(EXIT_FAILURE);
        }
    }

    return listener;
}

/**
 * initListen
 *
 * Initialisation de la procédure d'écoute
 *
 * @param   {SOCKET}    listener    Le socket serveur
 * @param   {fd_set *}  master      La liste de socket à écouter
 * @param   {int *}     int         Le file descriptor max
 */
void initListen(SOCKET listener, fd_set *master, int *max) {
    int stdin_fd = fileno(stdin);

    //Ajout du socket serveur dans la liste
    FD_SET(listener, master);
    DEBUG("[server] Adding listener %d to master", listener);
    //Ajout de stdin dans la liste
    FD_SET(stdin_fd, master);
    DEBUG("[server] Adding stdin %d to master", stdin_fd);

    *max = listener; //au début c'est le socket serveur qui a le plus grand fd
}


/**
 * selectSocket
 *
 * Éxécute select() sur la liste `master`
 *
 * @param  {fd_set *}   master      La liste de socket à attendre en lecture
 * @param  {fd_set *}   result      La liste de socket prêts à lire
 * @param  {int}        max         Le plus grand fd du lot
 */
void selectSocket(fd_set *master, fd_set *result, int max) {

    int nbRead = 0;

    DEBUG("[server] Select (max :%d)", max + 1);

    *result = *master;
    //On attent les socket de master prêts en lecture
    if ((nbRead = select(max + 1, result, NULL, NULL, NULL)) == -1) {
        perror("select error");
        exit(EXIT_FAILURE);
    }

    DEBUG("[server] %d file descriptors ready", nbRead);
}

/**
 * acceptConnection
 *
 * Accepter le socket fd et l'ajouter à la liste des sockets à lire
 *
 * @param  {SOCKET}     fd      Le socket à accepter
 * @param  {fd_set *}   master  La liste de sockets à lire
 * @param  {int *}      max     Le fd max parmis master
 *
 * @return {int}        Le socket du client ou -1
 */
int acceptConnection(SOCKET fd, fd_set *master, int *max) {

    int client_sock = -1;

    //info du client
    char   remoteIP[INET6_ADDRSTRLEN];
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    //on accepte la connextion
    client_sock = accept(
        fd,
        (struct sockaddr *) &client_addr,
        &client_addr_len
    );

    if (client_sock < 0) {
        perror("ERROR on accept");
        exit(EXIT_FAILURE);
    }

    //update du socket max si nécessaire
    if (client_sock > *max) {
        *max = client_sock;
    }

    //ajout du socket client dans la liste des socket à écouter
    FD_SET(client_sock, master);

    DEBUG("[server] new connection from %s on socket %d",
        inet_ntop( //trouver son ip
            client_addr.ss_family,
            getAdresse((struct sockaddr*) &client_addr),
            remoteIP,
            INET6_ADDRSTRLEN),
        client_sock
    );

    return client_sock;

}

/**
 * isSocket
 *
 * Helper pour tester si un file descriptor pointe sur un socket
 *
 * @param  {int}    fd
 *
 * @return {bool}   true si oui
 */
bool isSocket(int fd) {
    struct stat statbuf;
    fstat(fd, &statbuf);
    return S_ISSOCK(statbuf.st_mode) != 0;
}

/**
 * getLineSocket
 *
 * getline() pour un socket
 *
 * Grosse supposition : on trouvera toujours un caractère \n dans le flux du
 * socket. C'est le cas dans notre protocole, mais c'est loin d'être 100% safe
 *
 * @see Adapté de http://man7.org/tlpi/code/online/book/sockets/read_line.c.html
 *
 * @param  {char **}    line    Un pointeur vers le futur pointeur sur la ligne
 * @param  {size_t *}   size    Un pointeur vers le futur nombre de caractères
 *                              lus. (Pour copier la signature de getline())
 * @param  {int}        fd      Le file descriptor vers le socket
 *
 * @return {ssize_t}    Nombre d'octet lus
 */
ssize_t getLineSocket(char **line, size_t *size, int fd) {

    const int BLOCK_SIZE = 2; //une taille random pour l'allocation du buffer
    char *buf = (char *) malloc((BLOCK_SIZE + 1) * sizeof(char)); // +1 pour \0
    if (buf == NULL) {
        perror("Malloc error");
        exit(EXIT_FAILURE);
    }
    int blocks = 1;  //nombre de blocs utilisé pour buf

    size_t  totRead; //nombre total d'octets lus à l'instant t
    ssize_t numRead; //nombre d'octets lus par read()

    char ch;

    totRead = 0;
    while (true) {  //on fait la supposition qu'on trouvera toujours le
                    //caractère '\n'

        numRead = read(fd, &ch, sizeof(char));

        if (numRead == -1) {
            if (errno == EINTR) {  //Interruption, restart read()
                continue;
            } else {
                perror("Socket error");
                exit(EXIT_FAILURE);
            }
        } else if (numRead == 0) { //End of file
            if (totRead == 0) {    //on a rien lu
                return 0;
            } else {
                break; //on arrête la lecture
            }
        } else { //on a lu des choses

            //il faut peut-être alouer de la place au buffer
            if ((totRead + numRead) > blocks * BLOCK_SIZE) {
                blocks++;
                buf = (char *) realloc(buf, blocks * BLOCK_SIZE * sizeof(char));
                if (buf == NULL) {
                    perror("Realloc error");
                    exit(EXIT_FAILURE);
                }
            }

            //on copie ce qu'on a lu dans le buffer a la bonne place
            buf[totRead] = ch;
            totRead += numRead;

            if (ch == '\n') { //on a notre fin de ligne
                break;
            }
        }
    }

    *line = buf;
    *size = totRead;
    return totRead;
}

/**
 * readInputServer
 *
 * Read command from fd and process it
 */
int readInputServer(int fd) {

    size_t n;
    char *line = NULL;

    int fdInput, fdOutput;

    if (isSocket(fd)) { //c'est un socket
        n = getLineSocket(&line, &n, fd);

        fdInput  = fd;
        fdOutput = fd;
    } else { //c'est stdin
        n = getline(&line, &n, stdin);

        fdInput  = fileno(stdin);
        fdOutput = fileno(stdout);
    }

    DEBUG("[server] Received : %s", line);

    if (n == 0) { //End of file
        dprintf(fd, "\nBye !\n");
        if (fd == fileno(stdin)) {
            exit(EXIT_FAILURE);
        } else {
            close(fd);
        }
    } else {
        DEBUG("User: %s", line);
        process(line, fdInput, fdOutput);
        DEBUG("[server] end of process");
        printPrompt(fd);
    }

    free(line);

    return n;
}

/**
 * loopServer
 *
 * Boucle principale de l'entrée.
 * Création d'un serveur et écoute les connexions entrantes et stdin.
 * Si une connexion socket ou stdin est prêt à lire, appel de readFd
 */
void loopServer() {

    int port = -1;
    int socketServeur = -1; //le socket d'écoute de connexions
    if ((socketServeur = initSocket(&port)) == -1) {
        perror("Can't create socket");
        exit(EXIT_FAILURE);
    }

    DEBUG("[server] socket serveur fd=%d on port %d", socketServeur, port);

    printf("Listening on port %d\n", port);
    printPrompt(fileno(stdin));

    //Écoute

    fd_set socketsToRead; //la liste de socket à lire
    int maxSocket;        //le plus grand fd parmis socketsToRead
    initListen(socketServeur, &socketsToRead, &maxSocket);

    while (42) {

        fd_set readyToRead;
        selectSocket(&socketsToRead, &readyToRead, maxSocket);

        //On regarde la plage où se trouvent tous nos file descriptor
        for (int fd = 0; fd <= maxSocket; fd++) {

            if (FD_ISSET(fd, &readyToRead)) { //fd est prêt à être lu

                //une nouvelle connection sur le serveur
                if (fd == socketServeur) {
                    int sock = acceptConnection(fd, &socketsToRead, &maxSocket);
                    printPrompt(sock);
                } else { //Une entrée

                    DEBUG("[server] Reading input %d", fd);

                    int n = readInputServer(fd);

                    if (n == 0) {
                        FD_CLR(fd, &socketsToRead);
                        close(fd);
                    }
                }
            }
        }

    }
}
