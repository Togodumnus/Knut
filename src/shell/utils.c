#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#ifdef __APPLE__
    #include <limits.h>
#else
    #include <linux/limits.h>
#endif

#include "utils.h"

extern char *optchar;

/**
 * fileExtension
 *
 * @param  {char *}  file       Le fichier dont il faut trouver l'extension
 * @return {char *}             NULL si pas d'extension trouvée
 *                              Un pointeur sur le premier caractère de
 *                              l'extension sinon
 */
char *fileExtension(char *file) {

    //on cherche le point dans file
    char *point = strrchr(file, '.');

    //si pas de point ou point au début
    if (point == NULL || point == file) {
        return NULL;
    }

    //on retourne le pointeur sur le premier caractère de l'extension
    return point + 1;
}

/**
 * updatePATH
 *
 * Ajout d'un élément au $PATH (devant)
 * ie. utile pour ajouter le répertoir vers nos exécutable
 *
 * @see main.c
 * @param  {char *} prefix      La chaîne a ajouter devant le $PATH existant
 */
void updatePATH(const char* prefix) {

    char *newPATH = (char *) malloc(PATH_MAX * sizeof(char));
    if (newPATH == NULL) {
        perror("Malloc error");
        exit(1);
    }

    sprintf(newPATH, "PATH=%s:%s", prefix, getenv("PATH"));

    putenv(newPATH);
}

int readArgs(int argc, char *argv[]) {
    char option = 0;

    while ((option = getopt(argc, argv,"m:")) != -1) {
        if (option == 'm') {
            if (strcmp("exec", optarg) == 0) {
                return EXECUTABLE_MODE;
            } else if (strcmp("static", optarg) == 0) {
                return LIB_STATIC_MODE;
            } else if (strcmp("dynamic", optarg) == 0) {
                return LIB_DYNAMIC_MODE;
            }
        }
    }

    return EXECUTABLE_MODE;
}
