#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>

int indent = 0;

const int BUFFER_SIZE = 1024;

const int FOLDER_RIGHT = 0775;

/**
 * mkdirFull
 *
 * @param  {char *} path        Le dossier à créer
 */
void mkdirFull(char *path) {
    struct stat st;

    if (stat(path, &st) == -1) {
        if (mkdir(path, FOLDER_RIGHT) == -1) {
            perror("Can't create dir");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * kcp_file_to_file
 *
 * Copie le contenu d'un fichier dans un autre
 *
 * @param  {char *}     path1   La source
 * @param  {char *}     path2   La destination
 */
int kcp_file_to_file(char *path1, char *path2) {

    FILE * f1 = fopen(path1, "r");
    FILE * f2 = fopen(path2, "w");

    if (f1 == NULL) {
        printf("Can't open %s\n", path1);
        exit(EXIT_FAILURE);
    } else if (f2 == NULL) {
        printf("Can't open %s\n", path2);
        exit(EXIT_FAILURE);
    }

    //la copie
    ssize_t n;
    char buf[BUFFER_SIZE];
    while ((n = read(fileno(f1), buf, BUFFER_SIZE)) > 0 ) {
        ssize_t nw = 0;
        do {
            nw = write(fileno(f2), buf + nw, n - nw);
            if (nw == -1) {
                printf("Write error\n");
                break;
            }
        } while (nw != n);
    }

    if (n == 1) {
        printf("Read error\n");
    }

    fclose(f1);
    fclose(f2);
    return 0;
}

/**
 * kcp_file_to_dir
 *
 * Copie un fichier dans un répertoire
 *
 * @param  {char *}     file_path   La source
 * @param  {char *}     dir_path   La destination
 */
int kcp_file_to_dir(char *file_path, char *dir_path) {
    FILE * f;

    // au cas ou l'utilisateur a entré le chemin sans / à la fin
    char *dir = dir_path;

    //On crée le path vers le fichier destination
    char *fileName = basename(file_path);
    char file_pathFull[strlen(dir) + strlen(fileName)];
    strcpy(file_pathFull, dir);
    strcpy(file_pathFull + strlen(dir), fileName);

    if ((f = fopen(file_pathFull, "w")) == NULL) { // création du fichier
        printf("Can't open %s\n", file_pathFull);
        exit(EXIT_FAILURE);
    }

    fclose(f);

    //On copie le fichier dans le nouveau fichier créé
    return kcp_file_to_file(file_path, file_pathFull);
}

/**
 * kcp_files_to_dir
 *
 * Copie plusieurs fichiers en entrée dans un répertoire
 *
 * @param  {int *}        argc   Le nombre d'argument
 * @param  {char *}  argv   Les arguments
 */
int kcp_files_to_dir(int argc, char * argv[]) {
    struct stat st;
    int i;

    for (i = 1; i < argc-1; i++){
        //On vérifie ce n'est pas un dossier que l'on essaye de copier
        if (stat(argv[i], &st) == -1) {
            printf("error stat\n");
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(st.st_mode)) { //si c'est un répertoire, message erreur
            printf("kcp: omitting directory %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }

        kcp_file_to_dir(argv[i], argv[argc-1]);
    }
    return 0;
}

/**
 * kcp_dir_to_dir
 *
 * Copie le contenu d'un répertoire dans un répertoire
 *
 * @param  {char *}  dir_path_src    Le chemin du répertoire à copier
 * @param  {char *}  dir_path_dest   Le chemin du répertoire de destination
 */
int kcp_dir_to_dir(char *dir_path_src, char *dir_path_dest) {

    DIR *dirp_src;
    struct dirent *dptr_src;

    //ajout d'un / à la destination
    char dir_path_dest_tmp[strlen(dir_path_dest) + 2];
    strcpy(dir_path_dest_tmp, dir_path_dest);
    strcat(dir_path_dest_tmp, "/");
    mkdirFull(dir_path_dest_tmp);

    //ajout d'un / à la source
    char dir_path_src_tmp[strlen(dir_path_src) + 100];
    strcpy(dir_path_src_tmp, dir_path_src);
    strcat(dir_path_src_tmp, "/");

    if ((dirp_src = opendir(dir_path_src_tmp)) == NULL) {
        printf("Can't open directory %s %s\n", dir_path_src_tmp, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while ((dptr_src = readdir(dirp_src)) != NULL) { //tous les éléments de la source

        if ((strcmp(dptr_src->d_name, "..") != 0)
                && (strcmp(dptr_src->d_name,".") != 0)) {
            struct stat st;

            char path[strlen(dir_path_src_tmp) + strlen(dptr_src->d_name) + 2];
            strcpy(path, dir_path_src);
            strcat(path, "/");
            strcat(path, dptr_src->d_name);

            //distinction fichiers et dossiers
            if (stat(path, &st) == -1) {
                printf("error stat\n");
                exit(EXIT_FAILURE);
            }
            if (S_ISDIR(st.st_mode)) { //répertoire
                //construction de la destination
                char destDir[strlen(dir_path_dest_tmp) + strlen(dptr_src->d_name) + 2];
                strcpy(destDir, dir_path_dest_tmp);
                strcat(destDir, dptr_src->d_name);
                strcat(destDir, "/");
                mkdirFull(dir_path_dest_tmp);

                indent++;
                kcp_dir_to_dir(path, destDir);
                indent--;
            } else { //fichier
                kcp_file_to_dir(path, dir_path_dest_tmp);
            }
        }
    }

    if(closedir(dirp_src) != 0) {
        perror("Closedir error");
        exit(1);
    }

    return 0;
}

/**
 * kcp_dirs_to_dir
 *
 * Copie plusieurs répertoires en entrée dans un repertoire
 *
 * @param  {int *}        argc   Le nombre d'argument
 * @param  {char *}  argv   Les arguments
 */
int kcp_dirs_to_dir(int argc, char *argv[]) {
    int i;
    for (i = 2; i < argc-1; i++){
        kcp_dir_to_dir(argv[i], argv[argc-1]);
    }
    return 0;
}

/**
 * kcp
 *
 * Fonction d'entrée pour cp
 * Permet d'appeler les bonnes fonctions et de traiter les options
 *
 * @param  {int *}        argc   Le nombre d'argument
 * @param  {char *}  argv   Les arguments
 */
int kcp(int argc, char *argv[]) {
    struct stat st;

    char c;
    while((c = getopt(argc, argv, "r")) != -1) {
        switch(c) {
            case 'r': //on veut copier un dossier en récurcif
                return kcp_dirs_to_dir(argc, argv);
                break;
            default:
            case '?': //option non reconnue
                perror("Unknow option");
                exit(EXIT_FAILURE);
        }
    }

    //on test le type de la destination
    if (stat(argv[argc-1], &st) != -1 && S_ISDIR(st.st_mode)) {
        return kcp_files_to_dir(argc, argv);
    } else {
        if (argc > 3) {
            printf("cp: target %s is not a directory\n", argv[argc-1]);
            exit(EXIT_FAILURE);
        }
        return kcp_file_to_file(argv[1], argv[2]);
    }
}