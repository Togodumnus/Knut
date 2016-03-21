#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#ifdef __APPLE__
    #include <limits.h>
#else
    #include <linux/limits.h>
#endif

#include "libs.h"

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
CommandeFonction commandesFonctions[sizeof(commandes) / sizeof(char) - 1];
#else
CommandeFonction commandesFonctions[sizeof(commandes) / sizeof(char) - 1] =
    {yesLib};
#endif

 /**
 *
 * Charge la librairie `lib` et attache sa fonction <lib>Lib() sur
 * pCommandeFonction
 *
 * @param   {char *}                lib
 * @param   {CommandeFonction *}    pCommandeFonction
 */
void loadLib(char *lib, CommandeFonction *pCommandeFonction) {

    void *libFile;

    char *path     = (char *) malloc(PATH_MAX * sizeof(char));
    char *funcName = (char *) malloc(PATH_MAX * sizeof(char));

    sprintf(path, "./bin/libs/lib%sD.so", lib);
    sprintf(funcName, "%sLib", lib);

    if ((libFile = dlopen(path, RTLD_LAZY)) == NULL) {
        perror("lib introuvable");
        exit(1);
    }

    if ((*pCommandeFonction = (CommandeFonction) dlsym(libFile, funcName)) == NULL) {
        perror("fonction introuvable dans la lib");
        exit(1);
    }

    free(path);
    free(funcName);

}

/**
 * loadLibs
 *
 * Charge les librairies de `libs` et attache les fonctions <lib>Lib() sur
 * commandesFonctions.
 */
void loadLibs() {

    int i = 0;
    while (commandes[i] != NULL) { //la dernière libs doit être le pointeur NULL
        loadLib(commandes[i], &commandesFonctions[i]);
        i++;
    }

}

/**
 * findCommande
 *
 * Recherche si la commande cmd fait partie des librairies qui sont disponible
 * et retourne le pointeur vers <cmd>Lib si possible
 *
 * @param   {char *}    cmd     La commande qu'il faut trouver
 *
 * @return  {CommandeFonction}  Le pointeur vers <cmd>Lib ou NULL si la lib
 *                              n'est pas trouvée
 */
CommandeFonction findCommande(char *cmd) {

    int i = 0;
    while (commandes[i] != NULL) { //la dernière libs doit être le pointeur NULL
        if (strcmp(commandes[i], cmd) == 0) {
            return commandesFonctions[i];
        }
        i++;
    }

    return NULL;
}

