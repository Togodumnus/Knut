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

//flags
const int F_REC   = 1<<1;   //-r, -R
const int F_VERB  = 1<<2;   //-v
char *endptr;

/**
 * usage
 */
void usage() {
    printf("\
Knut chown\n\n\
usage: chown [-Rv] new_owner file ...\n\
\n\
\t-R\tRecursive\n\
\t-v\tVerbose\n\
");
}

void chownElem(char *path, uid_t uid, char *user);

/**
 * chownDirContent
 *
 * Parcours du contenu d'un répertoire pour chown
 *
 * @param  {char *}     path
 * @param  {int *}      uid
 */
void chownDirContent(char *path, uid_t uid){
    //TODO
}

/**
 * chownElem
 *
 * Modification des permissions de path
 *
 * @param  {char *}     path
 * @param  {int *}      uid
 * @param  {char *}     user
 */
void chownElem(char *path, uid_t uid, char *user){
    struct passwd *pwd;

    if (*endptr != '\0') {         /* Was not pure numeric string */
        pwd = getpwnam(user);   /* Try getting UID for username */
        if (pwd == NULL) {
            printf("Erreur\n");
        }
        uid = pwd->pw_uid;
    }

    if (chown(path, uid, -1) == -1) {
        printf("Erreur\n");
    }
}

/**
 * chownLib
 *
 * Fonction d'entrée
 *
 * @param  {int}    argc
 * @param  {char *} argv
 */
int chownLib(int argc, char *argv[]) {
    uid_t uid;
    int options = 0;

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
    char *user = argv[1];
    uid = strtol(argv[1], &endptr, 10);
    for (int i = optind+1; i < argc; i ++) {
        chownElem(argv[i], uid, user);
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
    enregisterCommande("chown", chownLib);
}