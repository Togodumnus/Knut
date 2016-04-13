#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <errno.h>
#include "../rm/rm.h"
#include "../cp/cp.h"



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

/**
 * Déplace un fichier/dossier dans un dossier 
 * 
 *@param  {char *} path_to_move        Le fichier/dossier à déplacer
 *@param  {char *} dir_path            Le dossier ou est déplacé le fichier/dossier
 *
 */
int kmv_one_to_dir(char * path_to_move, char * dir_path) {
    char * pathFull = (char *) malloc(strlen(path_to_move) + strlen(dir_path));
    struct stat st;
    if (lstat(path_to_move, &st) == -1) {
        perror("kmv");
        exit(EXIT_FAILURE);
    }

    if (dir_path[strlen(dir_path)-1] != '/') {
        strcat(dir_path, "/");
    }
    
    strcat(pathFull, dir_path);
    strcat(pathFull, basename(path_to_move));


    if (rename(path_to_move, pathFull) == -1) {
        if (errno == EXDEV) 
            copy_and_delete(path_to_move, dir_path);
        else 
            exit(EXIT_FAILURE);
    }

    free(pathFull);

    return 0;

}


/**
 * Déplace plusieurs fichiers/dossiers dans un dossier 
 * 
 *@param  {int}          Nombre d'arguments     
 *@param  {char * const} Les arguments         
 *
 */
int kmv_some_to_dir(int argc, char * argv[]) {
    int i;
    for (i = 1; i < argc-1; i++) {
        kmv_one_to_dir(argv[i], argv[argc-1]);
    }
    return 0;
}

/**
 * kmv
 *
 * Fonction d'entrée pour kmv
 * Permet d'appeler les bonnes fonctions en fonction des entrées
 *
 * @param  {int *}   argc   Le nombre d'argument
 * @param  {char *}  argv   Les arguments
 */
int kmv(int argc, char * argv[]) {
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
            exit(EXIT_FAILURE);
        }
    }
    else if (S_ISDIR(st.st_mode)) { // deplacer de fichier dans un repertoire
        return kmv_some_to_dir(argc, argv);
    }
    
    return 0;
}