#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#ifdef __APPLE__
    #include <limits.h>
#else
    #include <linux/limits.h>
#endif

#include "dynamicLib.h"

/**
 * loadLib
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
 *
 * @param   {char *[]}                  libs                Liste des commandes
 *                                                          à charger.
 * @param   {int (*[])(int, char *[])}  commandesFonctions  Liste des pointeurs
 *                                                          sur les fonctions
 *                                                          <lib>Lib()
 */
void loadLibs(char *libs[], int (*commandesFonctions[])(int, char *[])) {

    int i = 0;
    do {
        loadLib(libs[i], &commandesFonctions[i]);
        i++;
    } while (libs[i] != NULL); //la dernière libs doit être le pointeur NULL

};

