#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "utils.h"

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
    optind = 1;

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
            fprintf(stderr, "cp: target %s is not a directory\n", argv[argc-1]);
            exit(EXIT_FAILURE);
        }
        return kcp_file_to_file(argv[1], argv[2]);
    }
}

