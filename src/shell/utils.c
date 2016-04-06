#include <string.h>

#include "utils.h"

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
