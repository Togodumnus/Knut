#include <stdio.h>
#include <stdlib.h>
#include "../libs/yes/yes.h"

//si pas de LIB_STATIC, on charge les librairies dynamiquement
#ifndef LIB_STATIC
    #include "dynamicLib.h"
#endif

#include "front.h"
#include "process.h"

/**
 * commandes
 *
 * Liste des commandes disponibles par libraries statiques ou dynamiques
 * Ne pas oublier NULL à la fin du tableau.
 *
 * @see dynamicLib.c#loadLibs()
 */
char *commandes[] = {"yes", NULL};

/**
 * commandesFonctions
 *
 * Liste des fonctions XXXLib() d'entrée sur les librairies statiques ou
 * dynamiques disponnibles.
 * Doit être dans le même ordre que `char *commandes`.
 *
 * @see dynamicLib.c#loadLibs()
 */
#ifndef LIB_STATIC
int (*commandesFonctions[sizeof(commandes) / sizeof(char) - 1])(int, char *[]);
#else
int (*commandesFonctions[sizeof(commandes) / sizeof(char) - 1])(int, char *[]) =
    {yesLib};
#endif

int main(int argc, char* argv[]) {

    printf("KnutShell");

    #ifndef LIB_STATIC
    //si pas de LIB_STATIC, on charge les librairies dynamiquement
    loadLibs(commandes, commandesFonctions);
    printf(" (librairies dynamiques)\n");
    #else
    printf(" (librairies statiques)\n");
    #endif

    char *line = NULL;
    while (42) {
        printPrompt();
        if (readLine(&line) == -1) { //End of file
            printf("\nBye !\n");
            free(line);
            exit(1);
        } else {
            printf("User: %s", line);
            process(line);
        }
    }

}
