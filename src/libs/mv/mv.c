#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h> 

int kmv_file_to_dir(char * file, char * dir_path) {
    char * pathFull = (char *) malloc(strlen(file) + strlen(dir_path));
    struct stat st;
    if (lstat(file, &st)==-1) {
        printf("mv: cannot stat %s: No such file or directory\n", file);
        exit(EXIT_FAILURE);
    }

    if (dir_path[strlen(dir_path)-1]!='/') {
        strcat(dir_path, "/");
    }
    
    strcat(pathFull, dir_path);
    strcat(pathFull, file);

    rename(file, pathFull);
    return 0;

}

int kmv_files_to_dir(int argc, char * const argv[]) {
    int i;
    for (i = 1; i < argc-1; i++){
        kmv_file_to_dir(argv[i],argv[argc-1]);
    }
    return 0;
}

int kmv(int argc, char * const argv[]) {
    struct stat st;
    if (lstat(argv[argc-1], &st)==-1) { // renommage
        if (argc>3) {
            printf("mv: target %s is not a directory \n", argv[argc-1]);
            exit(EXIT_FAILURE);
        }
        rename(argv[1], argv[2]);
        return 0;
    }
    if (S_ISREG(st_mode)) { // renommage aussi
        if (argc>3) {
            printf("mv: target %s is not a directory \n", argv[argc-1]);
            exit(EXIT_FAILURE);
        }
        rename(argv[1], argv[2]);
    }
    else if (S_ISDIR(st.st_mode)) { // deplacer de fichier dans un repertoire
        return kmv_files_to_dir(argc, argv);
    }
    
    return 0;
}

 int main(int argc, char * const argv[]) {
    return kmv(argc, argv);
}