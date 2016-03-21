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
libFunc commandesFonctions[sizeof(commandes) / sizeof(char) - 1];
#else
libFunc commandesFonctions[sizeof(commandes) / sizeof(char) - 1] =
    {yesLib};
#endif

 /**
 *
 * Charge la librairie `lib` et attache sa fonction <lib>Lib() sur pFunc
 *
 * @param   {char *}                            lib
 * @param   {int (**)(int argc, char *argv[])}   pFunc
 */
void loadLib(char *lib, int (**pFunc)(int argc, char *argv[])) {

    void *libFile;

    char *path     = (char *) malloc(PATH_MAX * sizeof(char));
    char *funcName = (char *) malloc(PATH_MAX * sizeof(char));

    sprintf(path, "./bin/libs/lib%sD.so", lib);
    sprintf(funcName, "%sLib", lib);

    if ((libFile = dlopen(path, RTLD_LAZY)) == NULL) {
        perror("lib introuvable");
        exit(1);
    }

    if ((*pFunc = (libFunc) dlsym(libFile, funcName)) == NULL) {
        perror("fonction introuvable dans la lib");
        exit(1);
    }

    free(path);
    free(funcName);

};

/**
 * loadLibs
 *
 * Charge les librairies de `libs` et attache les fonctions <lib>Lib() sur
 * commandesFonctions.
 */
void loadLibs() {

    int i = 0;
    do {
        loadLib(commandes[i], &commandesFonctions[i]);
        i++;
    } while (commandes[i] != NULL); //la dernière libs doit être le pointeur NULL

};

