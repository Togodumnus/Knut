#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char* argv[]) {

    printf("KnutShell");

    #ifndef LIB_STATIC
        //si pas de LIB_STATIC, on charge les librairies dynamiquement
        printf(" (librairies dynamiques)\n");
        loadDynamicLibs(LIBS_DIR);
    #else
        enregisterCommande("yes", yesLib);
        printf(" (librairies statiques)\n");
    #endif
        showCommandes();

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
