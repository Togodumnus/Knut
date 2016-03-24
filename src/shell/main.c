#include <stdio.h>
#include <stdlib.h>
#include "libs.h"

#include "../DEBUG.h"
#include "../libs/yes/yes.h"

/**
 * LIBS_DIR
 *
 * Répertoire des librairies dynamiques
 */
char *LIBS_DIR = "./bin/libs/dynamic/";

int main(int argc, char* argv[]) {

    #ifndef LIB_STATIC
        //si pas de LIB_STATIC, on charge les librairies dynamiquement
        loadDynamicLibs(LIBS_DIR);
        printf("Dynamic\n");
    #else
        //En librairie statique, on connait les fonction a charger
        enregisterCommande("yes", yesLib);
        printf("Static\n");
    #endif

    printf("Hello world\n");

    (*findCommande("yes"))(argc, argv);

}
