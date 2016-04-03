#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>

const int BUFFER_SIZE = 1024;

const int FOLDER_RIGHT = 0775;

/**
 * mkdirFull
 *
 * @param  {char *} path        Le dossier à créer
 */
void mkdirFull(const char *path) {
    struct stat st;

    if (stat(path, &st) == -1) {
        if (mkdir(path, FOLDER_RIGHT) == -1) {
            perror("Can't create dir");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * appendToStr
 *
 * Ajout de ch à la fin de str
 *
 * @param  {char *}     str     La chaîne à compléter
 * @param  {char}       ch      Le caractère à ajouter
 *
 * @return {char *}             Un pointeur vers une nouvelle chaîne
 */
char *appendToStr(const char *str, char ch) {
    int l = strlen(str);

    char *newStr = (char *) malloc((l + 2) * sizeof(char));
    strcpy(newStr, str);
    if (newStr == NULL) {
        perror("Malloc error");
        exit(EXIT_FAILURE);
    }

    newStr[l] = ch;
    newStr[l+1] = '\0';

    return newStr;
}

/**
 * kcp_file_to_file
 *
 * Copie le contenu d'un fichier dans un autre
 *
 * @param  {char *}     path1   La source
 * @param  {char *}     path2   La destination
 */
int kcp_file_to_file(const char *path1, const char *path2) {

    FILE * f1 = fopen(path1, "r");
    FILE * f2 = fopen(path2, "w");

    printf("kcp_file_to_file : %s -> %s\n", path1, path2);

    if (f1 == NULL) {
        printf("Can't open %s\n", path1);
        exit(EXIT_FAILURE);
    } else if (f2 == NULL) {
        printf("Can't open %s\n", path2);
        exit(EXIT_FAILURE);
    }

    //la copie
    int n;
    char buf[BUFFER_SIZE];
    while ((n = read(fileno(f1), buf, BUFFER_SIZE)) > 0 ) {
        int nw = 0;
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
int kcp_file_to_dir(char *file_path, const char *dir_path) {
    FILE * f;

    printf("kcp_file_to_dir : %s -> %s\n", file_path, dir_path);

    // au cas ou l'utilisateur a entré le chemin sans / à la fin
    char *dir = appendToStr(dir_path, '/');

    //On crée le path vers le fichier destination
    char *fileName = basename(file_path);
    char file_pathFull[strlen(dir) + strlen(fileName)];
    strcpy(file_pathFull, dir);
    strcpy(file_pathFull + strlen(dir), fileName);
    free(dir);

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
 * @param  {char const*}  argv   Les arguments
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
int kcp_dir_to_dir(char *dir_path_src, const char *dir_path_dest) {
    DIR *dirp_src;
    struct dirent *dptr_src;
    struct stat st;

    char *dirName;

    // on ouvre le dossier sourc
    if ((dirp_src = opendir(dir_path_src)) == NULL) {
        printf("Can't open directory %s\n", dir_path_src);
        exit(EXIT_FAILURE);
    }

    //on ajoute un / à la fin du path si nécessaire
    char *dirDest = appendToStr(dir_path_dest, '/');
    mkdirFull(dirDest);

    //extraction du nom du dossier
    dirName = basename(dir_path_src);

    //Création du sous dossier dans la destination
    char destDir[strlen(dirName) + strlen(dirDest)];
    strcpy(destDir, dirDest);
    strcpy(destDir + strlen(dirDest), dirName);
    mkdirFull(destDir);

    printf("DESTDIR : %s\n", destDir);

    while ((dptr_src = readdir(dirp_src))) { //tous les éléments de la source

        // on ne tiens pas compte de . et ..
        if ((strcmp(dptr_src->d_name, "..") != 0)
                && (strcmp(dptr_src->d_name,".") != 0)) {

            int pathLength = strlen(dir_path_src)
                             + strlen(dptr_src->d_name) + 1;
            char path[pathLength];

            //construction du path vers l'élement d'origine
            if (dirName[strlen(dir_path_src)-1] == '/'){
                sprintf(path,"%s%s", dir_path_src, dptr_src->d_name);
            } else {
                sprintf(path,"%s/%s", dir_path_src, dptr_src->d_name);
            }
            printf("-> %p %s\n", &destDir, destDir);

            //distinction fichiers et dossiers
            if (stat(path, &st) == -1) {
                printf("path=%s\n", path); //TODO
                printf("error stat\n");
                exit(EXIT_FAILURE);
            }
            if (S_ISDIR(st.st_mode)) { // repertoire
                printf("Directory\n");
                kcp_dir_to_dir(path, destDir);
            } else { // fichier
                /*kcp_file_to_dir(path, destDir);*/
            }
        }
    }

    free(dirDest);
    closedir(dirp_src);

    return 0;
}

/**
 * kcp_dirs_to_dir
 *
 * Copie plusieurs répertoires en entrée dans un repertoire
 *
 * @param  {int *}        argc   Le nombre d'argument
 * @param  {char const*}  argv   Les arguments
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

int main(int argc, char *argv[]) {
    return kcp(argc, argv);
}
