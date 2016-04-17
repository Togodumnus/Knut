#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "utils.h"
#include "../../DEBUG.h"

const int RM_F_FORCE = 1;      //-f
const int RM_F_REC   = 1<<1;   //-r, -R
const int RM_F_VERB  = 1<<2;   //-v

/**
 * rmDirContent
 *
 * Parcours d'un dossier afin de supprimer tout son contenu
 *
 * @param  {char *} path
 * @param  {int}    options     Options à passer à rmElement
 */
void rmDirContent(char *path, int options) {

    DIR *dirp_src;
    struct dirent *dptr_src;

    if ((dirp_src = opendir(path)) == NULL) {
        fprintf(stderr, "Can't open directory %s : %s\n",
                path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    //tous les éléments de la source
    while ((dptr_src = readdir(dirp_src)) != NULL) {
        DEBUG("%s/%s", path, dptr_src->d_name);

        if ((strcmp(dptr_src->d_name, "..") != 0)
                && (strcmp(dptr_src->d_name, ".") != 0)) {

            char child_path[strlen(path) + strlen(dptr_src->d_name) + 2];
            strcpy(child_path, path);
            strcat(child_path, "/");
            strcat(child_path, dptr_src->d_name);

            rmElement(child_path, options); //on supprime l'élément
        }
    }

    if (closedir(dirp_src) != 0) {
        perror("Closedir error");
        exit(EXIT_FAILURE);
    }
}

/**
 * rmElement
 *
 * Suppression d'un fichier ou dossier (récurive si le flag est présent
 * dans options)
 *
 * @param  {char *} path
 * @param  {int}    options
 */
void rmElement(char *path, int options) {

    struct stat st;
    if (stat(path, &st) == -1) {
        if (errno == ENOENT) {
            perror("No such file");
            exit(EXIT_FAILURE);
        }
    }

    //si pas de -f, on vérifie que l'élement est accessible en écriture avant
    //de le supprimer
    char ok = 'n';
    if ((options & RM_F_FORCE) != RM_F_FORCE) {
        DEBUG("No force");
        if (access(path, W_OK) != 0) {
            printf("override permissions for %s ? [y/n] ", path);
            ok = getchar();
            printf("\n");
            fseek(stdin, 0L, SEEK_END);
            DEBUG("getchar : %c", ok);
        } else {
            ok = 'y';
        }
    } else {
        ok = 'y';
    }

    if (ok == 'y') {

        if ((options & RM_F_VERB) == RM_F_VERB) {
            printf("Deleting %s \n", path);
        }

        //si on est sur un dossier, on le supprime que si on a l'option -r
        if (S_ISDIR(st.st_mode)) {
            if ((options & RM_F_REC) == RM_F_REC) {
                DEBUG("Delete %s content", path);
                rmDirContent(path, options); //suppression du contenu

                DEBUG("rmdir %s", path);
                if (rmdir(path) == -1) {
                    fprintf(stderr, "Can't delete %s : %s\n", path, strerror(errno));
                    exit(EXIT_FAILURE);
                }
            } else {
                fprintf(stderr, "%s is a directory \n", path);
            }
        } else { //on est sur un fichier
            DEBUG("unlink %s", path);
            if (unlink(path) == -1) {
                fprintf(stderr, "Can't delete %s : %s\n", path, strerror(errno));
                exit(EXIT_FAILURE);
            }
        }

    }

};
