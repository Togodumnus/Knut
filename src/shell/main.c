#ifndef __APPLE__
    #define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>

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
void printPrompt() {
    printf("Toto @ KnutShell\n"); //TODO
    printf("> ");
    fflush(stdout);
}

void callbackInit(int port) {
    printf("Listening on port %d\n", port);
    printPrompt();
}

int readInputServer(int fd) {

    size_t n;
    char *line = NULL;

    if (isSocket(fd)) {
        n = getLineSocket(&line, &n, fd);
    } else { //c'est stdin
        n = getline(&line, &n, stdin);
    }

    DEBUG("[server] Received : %s", line);

    if (n == 0) { //End of file
        printf("\nBye !\n"); //TODO print to fd
        if (fd == fileno(stdin)) {
            exit(1);
        }
    } else {
        DEBUG("User: %s", line);
        process(line); //TODO, pass fd input and fd ouput in case of socket

        printPrompt(); //TODO print to fd
    }

    free(line);

    return n;
}

int readInputClient(char **msg) {

    size_t n;
    n = getline(msg, &n, stdin);

    DEBUG("[client] Read : %s", *msg);
    printPrompt();

    return n;
}

int main(int argc, char* argv[]) {

    printf("KnutShell");

    char *addr  = "localhost";
    int port    = -1;

    if (argc > 1) {
        readArgs(argc, argv, &EXEC_MODE, &addr, &port);
    }

    if (port > -1) { //on veut se connecter à un autre shell
        loopClient(addr, port);
        printf("Bye !\n");
    } else { //utilisation classique du shell

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

        loopServer(callbackInit, readInputServer);
    }
}
