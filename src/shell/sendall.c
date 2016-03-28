#include <sys/types.h>
#include <sys/socket.h>

#include "sendall.h"

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
