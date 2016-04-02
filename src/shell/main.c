#ifndef __APPLE__
    #define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"
#include "libs.h"
#include "process.h"
#include "server.h"
#include "client.h"

#include "../DEBUG.h"

#include "../libs/yes/yes.h"

/**
 * LIBS_DIR
 *
 * Répertoire des librairies dynamiques
 */
const char *LIBS_DIR = "./bin/libs/dynamic/";

/**
 * EXEC_DIR
 *
 * Répertoire de nos commandes exécutable
 */
const char *EXEC_DIR = "./bin/libs/";

/**
 * EXEC_MODE
 *
 * Le mode d'éxécution
 * @see utils.h
 *
 * Utiliser `extern` si besoin de le récupérer dans d'autre fichiers
 *
 */
enum execution_mode EXEC_MODE = EXECUTABLE_MODE;

/**
 * printPrompt
 *
 * Affiche le prompt avant une commande
 */
void printPrompt(int fd) {
    dprintf(fd, GREEN "Toto" WHITE " @ " YELLOW "KnutShell\n" END); //TODO
    dprintf(fd, YELLOW "> " END); //TODO flush with fsync here ?

    char eot = 4; //EOT : end of transmission
    if (write(fd, &eot, sizeof(char)) == -1) {
        perror("Can't end transmission with file descriptor");
        exit(1);
    }
}

//TODO write doc
void callbackInit(int port) {
    printf("Listening on port %d\n", port);
    printPrompt(fileno(stdin));
}

//TODO write doc
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
            exit(1);
        } else {
            close(fd);
        }
    } else {
        DEBUG("User: %s", line);
        process(line, fdInput, fdOutput);
        DEBUG("[server] end of process");
        printPrompt(fd);
    }

    //free(line); //TODO

    return n;
}

//TODO write doc
int readInputClient(char **msg) {

    size_t n;
    n = getline(msg, &n, stdin);

    /*DEBUG("here");*/

    return n;
}

int main(int argc, char* argv[]) {


    char *addr  = "127.0.0.1";
    int port    = -1;

    if (argc > 1) {
        readArgs(argc, argv, &EXEC_MODE, &addr, &port);
    }

    if (port > -1) { //on veut se connecter à un autre shell
        printf("KnutShell to %s:%d\n", addr, port);
        loopClient(addr, port, readInputClient);
        printf("Bye !\n");
    } else { //utilisation classique du shell

        printf("KnutShell");

        if (EXEC_MODE == EXECUTABLE_MODE) {
            printf(" (exécutables)\n");
            updatePATH(EXEC_DIR);
        } else if (EXEC_MODE == LIB_DYNAMIC_MODE) { //chargement des librairies
                                                    //dynamiquement
            printf(" (librairies dynamiques)\n");
            loadDynamicLibs(LIBS_DIR);
            showCommandes();
        } else {                                    //enregistrement des librairies
                                                    //statiques
            enregisterCommande("yes", yesLib);
            printf(" (librairies statiques)\n");
            showCommandes();
        }

        loopServer(callbackInit, printPrompt, readInputServer);
    }
}
