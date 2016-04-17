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
#include <stdbool.h>
#include <ctype.h>
#include <grp.h>
#include <pwd.h>

#include "../../LIB.h"
#include "../../DEBUG.h"
#include "../chown/utils.h"

//flags
const int F_REC   = 1<<1;   //-r, -R
const int F_VERB  = 1<<2;   //-v
struct  group *gr, *getgrnam(), *getgrgid();
struct passwd *pwd;

/**
 * usage
 */
void usage() {
    printf("\
Knut chgrp\n\n\
usage: chgrp [-Rv] <group> file ...\n\
\n\
\t-R\tRecursive\n\
\t-v\tVerbose\n\
");
}

/**
 * chgrpLib
 *
 * Fonction d'entrée
 *
 * @param  {int}    argc
 * @param  {char *} argv
 */
int chgrpLib(int argc, char *argv[]) {
    uid_t uid = -1;
    gid_t gid;
    int options = 0;
    char *token;

    char c;
    while((c = getopt(argc, argv, "Rvf")) != -1) {
        switch(c) {
            case 'R': //on veut modifier en récursif
                DEBUG("-R option");
                options |= F_REC;
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
    token = argv[optind];
    /* on récupère le groupe */
    if (isNumber(token)) {
        gid = atoi(token);
        gr = getgrgid(gid);
        if (uid && gr == NULL){
            printf("%s: unknown group", token);
            exit(EXIT_FAILURE);
        }
    } else {
        gr = getgrnam(token);
        if (gr == NULL){
            printf("%s: unknown group", token);
            exit(EXIT_FAILURE);
        }
        gid = gr->gr_gid;
    }

    for (int i = optind+1; i < argc; i ++) {
        chElem(argv[i], options, uid, gid,F_REC,F_VERB);
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
    enregisterCommande("chgrp", chgrpLib);
}
