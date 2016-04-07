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

    struct sockaddr_in6 config;

    DEBUG("Connection to %s:%d", addr, port);

    SOCKET sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    //connection compatible IPV6 ET IPV4
    memset(&config, 0, sizeof(config));
    config.sin6_family = AF_INET6;
    memcpy(&config.sin6_addr, &in6addr_any, sizeof(in6addr_any));
    config.sin6_port = htons(port);

    if (connect(sock, (struct sockaddr *)&config,
            sizeof(struct sockaddr_in6)) < 0) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

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
