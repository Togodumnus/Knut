#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

#include "../../DEBUG.h"

//flags
const int CHMOD_F_FORCE = 1;      //-f
const int CHMOD_F_REC   = 1<<1;   //-r, -R
const int CHMOD_F_VERB  = 1<<2;   //-v

/**
 * octalDecimal
 *
 * @param  {int}    n
 */
int octalDecimal(int n) {
    int decimal=0, i=0;
    while (n!=0) {
        int rem = n%10;
        n/=10;
        decimal += rem*pow(8,i);
        ++i;
    }
    return decimal;
}

/**
 * usage
 */
void usageChmod() {
    printf("\
Knut chmod\n\n\
usage: chmod [-Rfv] [0-7] file(s) ...\n\
\n\
\t-R\tRecursive\n\
\t-v\tVerbose\n\
");
}

void chmodElem(char *path, int options, int perms);

/**
 * chmodDirContent
 *
 * Parcours du contenu d'un répertoire pour chmod
 *
 * @param  {char *}     path
 * @param  {int}        options
 * @param  {int}        perms
 */
void chmodDirContent(char *path, int options, int perms){

    DIR *dirp_src;
    struct dirent *dptr_src;

    if ((dirp_src = opendir(path)) == NULL) {
        fprintf(stderr, "Can't open directory %s : %s\n",
                path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while ((dptr_src = readdir(dirp_src)) != NULL) {
        DEBUG("%s/%s", path, dptr_src->d_name);

        if ((strcmp(dptr_src->d_name, "..") != 0)
                && (strcmp(dptr_src->d_name, ".") != 0)) {

            char child_path[strlen(path) + strlen(dptr_src->d_name) + 2];
            strcpy(child_path, path);
            strcat(child_path, "/");
            strcat(child_path, dptr_src->d_name);

            chmodElem(child_path, options, perms); //on modifie les droits de l'élément
        }
    }

    if (closedir(dirp_src) != 0) {
        perror("Closedir error");
        exit(EXIT_FAILURE);
    }
}

/**
 * chmodElem
 *
 * Modification des permissions de path
 *
 * @param  {char *}     path
 * @param  {int}        options
 * @paran  {int}        perms
 */
void chmodElem(char *path, int options, int perms){

    //infos sur l'élément
    struct stat st;
    if (stat(path, &st) == -1) {
        if (errno == ENOENT) {
            perror("No such file");
            exit(EXIT_FAILURE);
        }
    }

    //si pas de -f, on vérifie que l'élement est accessible en écriture avant
    //de modifier ses droits
    char ok = 'n';
    if ((options & CHMOD_F_FORCE) != CHMOD_F_FORCE) {
        DEBUG("No force");
        if (access(path, W_OK) != 0) {
            //demande yes/no
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

        if ((options & CHMOD_F_VERB) == CHMOD_F_VERB) {
            printf("Changing file %s permissions\n", path);
        }

        //si on est sur un dossier,
        if (S_ISDIR(st.st_mode)) {

            //On regarde si on a l'option -r
            if ((options & CHMOD_F_REC) == CHMOD_F_REC) {
                DEBUG("Chmod %s content", path);
                //modification des droits du contenu
                chmodDirContent(path, options, perms);
            }

        }

        DEBUG("chmod %s", path);
        if (chmod(path, (mode_t) perms) == -1) {
            fprintf(stderr,
                    "Can't modify permissions %s : %s\n", path,
                    strerror(errno));
            exit(EXIT_FAILURE);
        }

    }

}

/**
 * chmodLib
 *
 * Fonction d'entrée
 *
 * @param  {int}    argc
 * @param  {char *} argv
 */
int chmodLib(int argc, char *argv[]) {

    int options = 0;
    int perms = 0;
    optind = 1;

    char c;
    while((c = getopt(argc, argv, "Rvf")) != -1) {
        switch(c) {
            case 'R': //on veut modifier en récursif
                DEBUG("-R option");
                options |= CHMOD_F_REC;
                break;
            case 'f':
                DEBUG("-f option");
                options |= CHMOD_F_FORCE;
                break;
            case 'v':
                DEBUG("-v option");
                options |= CHMOD_F_VERB;
                break;
            case '?': //option non reconnue
            default:
                usageChmod();
                exit(EXIT_FAILURE);
        }
    }
    if (argc < 2) {
        usageChmod();
        exit(EXIT_FAILURE);
    }
    perms = atoi(argv[optind]); //on récupère les droits à la bonne place
    perms = octalDecimal(perms);
    for (int i = optind+1; i < argc; i ++) {
        chmodElem(argv[i], options, perms);
    }

    return 0;
}

