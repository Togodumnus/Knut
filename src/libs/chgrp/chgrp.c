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
 * isnumber
 * 
 * Parcours un char et retourne 1 si il est constitué que de chiffre
 *
 * @param {char *}      s
 */
int isnumber(char *s){
    int c;
    while((c = *s++))
        if (!isdigit(c))
            return 0;
    return 1;
}

void chgrpElem(char *path, int options, uid_t uid, gid_t gid);

/**
 * chgrpDirContent
 *
 * Parcours du contenu d'un répertoire pour chgrp
 *
 * @param  {char *}     path
 * @param  {uid_t}      uid
 * @param  {gid_t}      gid
 */
void chgrpDirContent(char *path, int options, uid_t uid, gid_t gid){
    chDirContent(char *path, int options, uid_t uid, gid_t gid);
}

/**
 * chgrpElem
 *
 * Modification des permissions de path
 *
 * @param  {char *}     path
 * @param  {uid_t}      uid
 * @param  {gid_t}      gid
 */
void chgrpElem(char *path, int options, uid_t uid, gid_t gid){
    if ((options & F_VERB) == F_VERB) {
            printf("Changing %s group\n", path);
    }
    chElem(char *path, int options, uid_t uid, gid_t gid);
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
    /* on récupère le groupe */
    if (isnumber(argv[optind])) {
        gid = atoi(optind);
        gr = getgrgid(gid);
        if (uid && gr == NULL){
            printf("%s: unknown group", optind);
            exit(EXIT_FAILURE);
        }
    } else {
        gr = getgrnam(optind);
        if (gr == NULL){
            printf("%s: unknown group", optind);
            exit(EXIT_FAILURE);
        }
        gid = gr->gr_gid;
    }
    
    for (int i = optind+1; i < argc; i ++) {
        chgrpElem(argv[i], options, uid, gid);
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