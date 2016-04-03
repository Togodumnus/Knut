#ifndef __APPLE__
    #define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

#include "../DEBUG.h"

#include "server.h"
#include "client.h"

/**
 * sendall
 *
 * Helper pour s'assurer qu'on envoie le message dans sa totalité.
 * send() peut ne pas tout envoyer. Comme il retourne le nombres d'octets
 * envoyés, on peut refaire un send avec les octets restants.
 *
 * @see http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 *
 * @param   {int}       socket
 * @param   {char *}    buffer      Le message à envoyer
 * @param   {int}       length      La taille du message
 * @param   {int}       flag        Un flag pour send()
 *
 * @return  {int}       -1 si erreur, le nombre d'octets envoyés sinon sinon
 */
int sendall(int socket, char *buffer, int length, int flag) {
    int total    = 0,      //nombre d'octets envoyés
        restants = length; //nombre d'octets qu'il reste à envoyer
    int n;

    while (total < length) {
        if ((n = send(socket, buffer + total, restants, flag)) == -1) {
            break;
        }
        total    += n;
        restants -= n;
    }

    return n == -1 ? -1 : total;
}

/**
 * createClientSocket
 *
 * Création d'un socket vers la machine add:port
 *
 * @param  {char *} addr
 * @param  {int}    port
 *
 * @return {int}    Le socket
 */
int createClientSocket(char *addr, int port) {

    int status;
    struct addrinfo hints = {};      //config
    struct addrinfo *servinfo;       //info serveur

    char *port_ch = (char *) malloc(6 * sizeof(char));
    if (port_ch == NULL) {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    sprintf(port_ch, "%d", port);

    hints.ai_family   = AF_UNSPEC;   //IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM; //TCP
    hints.ai_flags    = AI_PASSIVE;  //remplir automatiquement mon ip

    if ((status = getaddrinfo(addr, port_ch, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    DEBUG("Connection to %s:%d", addr, port);

    SOCKET sock = socket(
        servinfo->ai_family,
        servinfo->ai_socktype,
        servinfo->ai_protocol
    );

    if (sock == -1) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    //On met le socket en état non bloquant pour que recv ne soit pas bloquant

    freeaddrinfo(servinfo); // free the linked-list

    return sock;
}

/**
 * readInputClient
 *
 * Read stdin command to msg
 *
 * @param  {char **}    msg
 */
int readInputClient(char **msg) {

    size_t n;
    n = getline(msg, &n, stdin);

    return n;
}

/**
 * loopClient
 *
 * Boucle principale de l'entrée.
 * Création d'un client, écoute et envoie des commandes vers un shel distant
 *
 * @param  {char *}                 addr
 * @param  {int}                    port
 */
void loopClient(char *addr, int port) {

    const char eot = 4;
    int socketClient = createClientSocket(addr, port);

    while (42) {

        DEBUG("[client] Waiting for message");

        //On lit un message jusqu'à un EOT
        char ch; //caractère par caractère
        int result = -1;
        while ((result = read(socketClient, &ch, sizeof(char))) > 0) {
            printf("%c", ch);
            if (ch == eot) {
                break;
            }
        }

        if (result == -1) { //si erreur autre que pas de data
            perror("reception error");
        }

        DEBUG("[client] end of response");

        //On envoi un message
        char *msg = NULL;
        int n = readInputClient(&msg); //entrée utilisateur

        DEBUG("[client] Sending %s", msg);

        if (n > 0) {
            //envoi de la commande
            if (sendall(socketClient, msg, strlen(msg), 0) == -1) {
                perror("sendall() error");
            }
        } else {
            break;
        }
    }

}
