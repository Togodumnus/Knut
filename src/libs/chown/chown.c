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

#include "../../DEBUG.h"
#include "utils.h"

//flags
const int CHOWNF_REC   = 1<<1;   //-r, -R
const int CHOWNF_VERB  = 1<<2;   //-v
struct group *gr, *getgrnam(), *getgrgid();
struct passwd *pwd;

/**
 * usage
 */
void usageChown() {
    printf("\
Knut chown\n\n\
usage: chown [-Rv] <owner> [:<group>] file ...\n\
\n\
\t-R\tRecursive\n\
\t-v\tVerbose\n\
");
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
                options |= CHOWNF_REC;
                break;
            case 'v':
                DEBUG("-v option");
                options |= CHOWNF_VERB;
                break;
            case '?': //option non reconnue
            default:
                usageChown();
                exit(EXIT_FAILURE);
        }
    }
    if (argc < 2) {
        usageChown();
        exit(EXIT_FAILURE);
    }
    const char s[2] = ":";
    char *token;
    gid_t gid = -1;

    // on prend l'utilisateur
    token = strtok(argv[optind], s);
    if (isNumber(token)){
        uid = atoi(token);
    }
    else{
        pwd = getpwnam(token); //On essaye d'avoir l'UID par le username
        if (pwd == NULL) {
            printf("invalid username\n");
            exit(EXIT_FAILURE);
        }
        uid = pwd->pw_uid;
    }
    //on regarde si il y a d'autre token (le groupe)
    while(token != NULL){
        token = strtok(NULL, s);
        if(token != NULL){
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
        }
    }

    for (int i = optind+1; i < argc; i ++) {
        chElem(argv[i], options, uid, gid, CHOWNF_REC, CHOWNF_VERB);
    }

    return 0;
}

