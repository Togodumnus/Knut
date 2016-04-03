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

int main(int argc, char* argv[]) {

    char *addr  = "127.0.0.1";
    int port    = -1;

    if (argc > 1) {
        readArgs(argc, argv, &EXEC_MODE, &addr, &port);
    }

    if (port > -1) { //on veut se connecter à un autre shell
        printf("KnutShell to %s:%d\n", addr, port);
        loopClient(addr, port);
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

        loopServer();
    }
}
