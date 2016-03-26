#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "libs.h"
#include "front.h"
#include "process.h"

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

    printf("KnutShell");

    if (argc > 1) {
        EXEC_MODE = readArgs(argc, argv);
    }

    if (EXEC_MODE == EXECUTABLE_MODE) {
        printf(" (exécutables)\n");
        updatePATH(EXEC_DIR);
    } else if (EXEC_MODE == LIB_DYNAMIC_MODE) { //chargement des librairies
                                                //dynamiquement
        printf(" (librairies dynamiques)\n");
        loadDynamicLibs(LIBS_DIR);
    } else {                                    //enregistrement des librairies
                                                //statiques
        enregisterCommande("yes", yesLib);
        printf(" (librairies statiques)\n");
        showCommandes();
    }

    char *line = NULL;
    while (42) {
        printPrompt();
        if (readLine(&line) == -1) { //End of file
            printf("\nBye !\n");
            free(line);
            exit(1);
        } else {
            DEBUG("User: %s", line);
            process(line);
        }
    }

    printf("Hello world\n");
}
