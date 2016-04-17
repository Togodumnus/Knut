#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "utils.h"
#include "rm.h"

#include "../../DEBUG.h"

/**
 * usage
 */
void usageRm() {
    printf("\
Knut rm\n\n\
usage: rm [-frRv] file ...\n\
");
}

/**
 * rmLib
 *
 * @param  {int}        argc
 * @param  {char **}    argv
 */
int rmLib(int argc, char *argv[]) {

    int options = 0;
    optind = 1;

    char c;
    while((c = getopt(argc, argv, "rRvf")) != -1) {
        switch(c) {
            case 'R':
            case 'r': //on veut supprimer en récursif
                DEBUG("-r option");
                options |= RM_F_REC;
                break;
            case 'f':
                DEBUG("-f option");
                options |= RM_F_FORCE;
                break;
            case 'v':
                DEBUG("-v option");
                options |= RM_F_VERB;
                break;
            case '?': //option non reconnue
            default:
                usageRm();
                exit(EXIT_FAILURE);
        }
    }

    if (argc < 2) {
        usageRm();
        exit(EXIT_FAILURE);
    }

    for (int i = optind; i < argc; i++) {
        rmElement(argv[i], options);
    }

    return 0;
}

