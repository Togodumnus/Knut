#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <stdbool.h>
#include <errno.h>

#include "../rm/utils.h"
#include "../cp/utils.h"

#include "../../LIB.h"
#include "../../DEBUG.h"

const int SRC_REG = 0x1;
const int SRC_DIR = 0x2;

const int TYPE_FILE = 0;
const int TYPE_DIR  = 1;

/**
 * copy_file_and_delete
 *
 * Copie un dossier et supprime la source
 * Fonction appelé lorsque la source et la destination ne sont pas sur le même
 * disque
 *
 * @param  {char *} path_to_move        Le fichier à déplacer
 * @param  {char *} dir_path            Le dossier ou est déplacé le fichier
 */
void copy_file_and_delete(char * path_to_move, char * dir_path) {
    kcp_file_to_dir(path_to_move, dir_path);
    rmElement(path_to_move, F_REC | F_FORCE);
}

/**
 * copy_dir_and_delete
 *
 * Copie un dossier et supprime la source
 * Fonction appelé lorsque la source et la destination ne sont pas sur le même
 * disque
 *
 * @param  {char *} path_to_move        Le dossier à déplacer
 * @param  {char *} dir_path            Le dossier ou est déplacé le dossier
 */
void copy_dir_and_delete(char * path_to_move, char * dir_path) {
    kcp_dir_to_dir(path_to_move, dir_path);
    rmElement(path_to_move, F_REC | F_FORCE);
}

/**
 * getSourceType
 *
 * @param  {char *} source
 * @return {int}    -1 si source n'existe pas
 *                  SRC_DIR si source est un dossier
 *                  SRC_REG si source est un fichier
 *                  0 sinon
 */
int getSourceType(char *source) {

    DEBUG("getSourceType %s", source);

    struct stat sourceSt;
    if (stat(source, &sourceSt) == -1) {
        if (errno == ENOENT) { //target n'existe pas
            return -1;
        } else {
            perror("Stat error");
            exit(EXIT_FAILURE);
        }

    }
    if (S_ISDIR(sourceSt.st_mode)) {
        return SRC_DIR;
    } else if (S_ISREG(sourceSt.st_mode)) {
        return SRC_REG;
    }
    return 0;
}

/**
 * moveToDir
 *
 * @param  {char *} item    fichier ou dossier
 * @param  {int}    type    TYPE_FILE ou TYPE_DIR
 * @param  {char *} target  Où le déplacer
 */
void moveToDir(char *item, int type, char *target) {
    char path[strlen(target) + strlen(basename(item)) + 2];
    strcpy(path, target);
    strcat(path, "/");
    strcat(path, basename(item));

    DEBUG("Move item %s to %s", item, path);

    if (rename(item, path) == -1) {
        if (errno == EXDEV) { //on change de disque, rename ne fonctionne pas
                              //on fait une copie puis une suppression de la
                              //source
            if (type == TYPE_DIR) {
                DEBUG("copy dir %s to %s", item, target);
                copy_dir_and_delete(item, target);
            } else { //TYPE_FILE
                DEBUG("copy file %s to %s", item, target);
                copy_file_and_delete(item, target);
            }
        } else {
            perror("rename error");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * usage
 */
void usage() {
    printf("\
Knut mv \n\
\n\
usage: mv source target\n\
       mv source ... directory\n\
");
}

/**
 * kmv
 *
 * Fonction d'entrée pour kmv
 * Permet d'appeler les bonnes fonctions en fonction des entrées
 *
 * Deux cas :
 *  1. mv source target
 *     Renommage de source (fichier ou dossier) en target
 *  2. mv source ... dir
 *     Déplacement des fichiers source dans le dossier dir
 *
 * @param  {int *}   argc   Le nombre d'argument
 * @param  {char *}  argv   Les arguments
 */
int kmv(int argc, char * argv[]) {

    if (argc < 3) {
        usage();
        exit(EXIT_FAILURE);
    }

    //on regarde le dernier argument
    struct stat targetSt;
    bool   targetExist = true;

    if (stat(argv[argc-1], &targetSt) == -1) {
        if (errno == ENOENT) { //target n'existe pas
            targetExist = false;
        } else {
            perror("Stat error");
            exit(EXIT_FAILURE);
        }
    }

    //on regarde le 1er argument
    int sourceType = getSourceType(argv[1]);
    if (sourceType == -1) {
        fprintf(stderr, "Source %s doesn't exist\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    //la destination est un dossier
    if (targetExist && S_ISDIR(targetSt.st_mode)) {

        //on déplace tous les éléments pointés par les arguments de mv dans
        //target
        int i;
        for (i = 1; i < argc - 1; i++) {
            DEBUG("Move %s dans %s", argv[i], argv[argc-1]);
            int fileType = getSourceType(argv[i]);
            if (fileType == -1) {
                fprintf(stderr, "Source %s doesn't exist\n", argv[i]);
                exit(EXIT_FAILURE);
            } else if ((fileType & SRC_REG) == SRC_REG) {
                moveToDir(argv[i], TYPE_FILE, argv[argc - 1 ]);
            } else if ((fileType & SRC_DIR) == SRC_DIR) {
                moveToDir(argv[i], TYPE_DIR, argv[argc - 1]);
            } else {
                fprintf(stderr, "Source %s is not a file, not a directory\n",
                        argv[i]);
                exit(EXIT_FAILURE);
            }
        }

    } else if (argc == 3) { //renomage de source en target
        rename(argv[1], argv[2]);
    } else {
        usage();
        exit(EXIT_FAILURE);
    }
    return 0;
}


/**
 * Init
 *
 * S'enregistre dans le shell dans le cas d'un chargement de la librairie
 * dynamique
 */
void Init(EnregisterCommande enregisterCommande) {
    enregisterCommande("mv", kmv);
}
