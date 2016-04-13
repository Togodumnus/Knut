#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

#include "../rm/rm.h"
#include "../cp/cp.h"
#include <stdbool.h>
#include <errno.h>

#include "../../LIB.h"
#include "../../DEBUG.h"

const int SRC_REG = 0x1;
const int SRC_DIR = 0x2;


/**
 * Copie un fichier et supprime la source
 * Fonction appelé lorsque la source et la destination ne sont pas sur le même disque
 * 
 *@param  {char *} path_to_move        Le fichier/dossier à déplacer
 *@param  {char *} dir_path            Le dossier ou est déplacé le fichier/dossier
 *
 */
int copy_and_delete(char * path_to_move, char * dir_path) {
    kcp_file_to_dir(path_to_move, dir_path);
    rmElement(path_to_move, 0);
    return 0;
}


int getSourceType(char *source) {
    struct stat sourceSt;
    if (stat(source, &sourceSt) == -1) {
        if (errno == ENOENT) { //target n'existe pas
            return -1;
        } else {
            perror("Stat error");
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(sourceSt.st_mode)) {
            return SRC_DIR;
        } 
        else if (S_ISREG(sourceSt.st_mode)) {
            return SRC_REG;
        } 
        else {
            return 0;
        }
    }
}


/**
 * getSourceType
 *
 * @param  {char *} source
 * @return {int}    -1 si source n'existe pas sur le disque
 *                  SRC_DIR si source est un dossier
 *                  SRC_REG si source est un fichier
 *                  0 sinon
 */
int kmv_one_to_dir(char * path_to_move, char * dir_path) {
    char * pathFull = (char *) malloc(strlen(path_to_move) + strlen(dir_path));
    struct stat st;
    if (lstat(path_to_move, &st) == -1) {
        perror("kmv");
        exit(EXIT_FAILURE);
    }
    if (rename(path_to_move, pathFull) == -1) {
        if (errno == EXDEV) 
            copy_and_delete(path_to_move, dir_path);
        else 
            exit(EXIT_FAILURE);
    }
}

/**
 * moveToDir
 *
 * @param  {char *} item    File of directory
 * @param  {char *} target  Where to move it
 */
void moveToDir(char *item, char *target) {
    char path[strlen(target) + strlen(basename(item)) + 2];
    strcpy(path, target);
    strcat(path, "/");
    strcat(path, basename(item));

    DEBUG("Move item %s to %s", item, path);

    rename(item, path);
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
<<<<<<< HEAD
    struct stat st;
    if (lstat(argv[argc-1], &st) == -1){ // renommage
        if (strcmp(dirname(argv[argc-1]), ".")==0) {
            if (argc>3) {
                printf("kmv: target %s is not a directory \n", argv[argc-1]);
                exit(EXIT_FAILURE);
            }// pb ici, quand le dossier n'existe pas on doit le créer mais avec cette algo on rentre ici alors qu'on devrais pas
            if (rename(argv[1], argv[2]) == -1) {
                perror("kmv");
                exit(EXIT_FAILURE);
            }
        return 0;
        }
    }
    if (S_ISREG(st.st_mode)) { // renommage aussi
        if (argc>3) {
            printf("kmv: target %s is not a directory \n", argv[argc-1]);
            exit(EXIT_FAILURE);
        }
        if (rename(argv[1], argv[2]) == -1) {
            perror("kmv");
=======

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
>>>>>>> 38146a4612c49fc9bbf57b74c1941235516a45a3
            exit(EXIT_FAILURE);
        }
    }

    //on regarde le 1er argument
    int sourceType = getSourceType(argv[1]);
    if (sourceType == -1) {
        fprintf(stderr, "Source %s doesn't exist\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    if (targetExist && S_ISDIR(targetSt.st_mode)) { //la destination est un dossier

        //on déplace tous les éléments pointés par les arguments de mv dans
        //target
        for (int i = 1; i < argc - 1; i++) {
            DEBUG("Move %s dans %s", argv[i], argv[argc-1]);
            int fileType = getSourceType(argv[i]);
            if (fileType == -1) {
                fprintf(stderr, "Source %s doesn't exist\n", argv[i]);
                exit(EXIT_FAILURE);
            } else if ((fileType & SRC_REG) == SRC_REG) {
                moveToDir(argv[i], argv[argc - 1 ]);
            } else if ((fileType & SRC_DIR) == SRC_DIR) {
                moveToDir(argv[i], argv[argc - 1]);
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
