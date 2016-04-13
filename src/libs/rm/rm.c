#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "utils.h"
#include "rm.h"

#include "../../LIB.h"
#include "../../DEBUG.h"

/**
 * usage
 */
void usage() {
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

    char c;
    while((c = getopt(argc, argv, "rRvf")) != -1) {
        switch(c) {
            case 'R':
            case 'r': //on veut supprimer en récursif
                DEBUG("-r option");
                options |= F_REC;
                break;
            case 'f':
                DEBUG("-f option");
                options |= F_FORCE;
                break;
            case 'v':
                DEBUG("-v option");
                options |= F_VERB;
                break;
            case '?': //option non reconnue
            default:
                usage();
                exit(EXIT_FAILURE);
        }
    }

    if (argc < 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    for (int i = optind; i < argc; i++) {
        rmElement(argv[i], options);
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
    enregisterCommande("rm", rmLib);
}
