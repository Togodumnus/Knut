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
struct  group *gr, *getgrnam(), *getgrgid();
struct passwd *pwd;

/**
 * usage
 */
void usage() {
    printf("\
Knut chown\n\n\
usage: chown [-Rv] <owner> [:<group>] file ...\n\
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

void chownElem(char *path, int options, uid_t uid, gid_t gid);

/**
 * chownDirContent
 *
 * Parcours du contenu d'un répertoire pour chown
 *
 * @param  {char *}     path
 * @param  {uid_t}      uid
 * @param  {gid_t}      gid
 */
void chownDirContent(char *path, int options, uid_t uid, gid_t gid){
    DIR *dirp_src;
    struct dirent *dptr_src;

    if ((dirp_src = opendir(path)) == NULL) {
        fprintf(stderr, "Can't open directory %s : %s\n",
                path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while ((dptr_src = readdir(dirp_src)) != NULL) {
        DEBUG("%s/%s", path, dptr_src->d_name);

        if ((strcmp(dptr_src->d_name, "..") != 0)
                && (strcmp(dptr_src->d_name, ".") != 0)) {

            char child_path[strlen(path) + strlen(dptr_src->d_name) + 2];
            strcpy(child_path, path);
            strcat(child_path, "/");
            strcat(child_path, dptr_src->d_name);

            //on modifie l'utilisateur
            chownElem(child_path, options, uid, gid); 
        }
    }

    if (closedir(dirp_src) != 0) {
        perror("Closedir error");
        exit(EXIT_FAILURE);
    }
}

/**
 * chownElem
 *
 * Modification des permissions de path
 *
 * @param  {char *}     path
 * @param  {uid_t}      uid
 * @param  {gid_t}      gid
 */
void chownElem(char *path, int options, uid_t uid, gid_t gid){
    struct stat st;
    if (stat(path, &st) == -1) {
        if (errno == ENOENT) {
            perror("No such file");
            exit(EXIT_FAILURE);
        }
    }

    if ((options & F_VERB) == F_VERB) {
            printf("Changing %s owner\n", path);
    }
    if (S_ISDIR(st.st_mode)) {

        //On regarde si on a l'option -r
        if ((options & F_REC) == F_REC) {
            DEBUG("chown %s content", path);
            //modification des droits du contenu
            chownDirContent(path, options, uid, gid);
        }

    }

    DEBUG("chown %s", path);
    if (chown(path, uid, gid) == -1){
        fprintf(stderr,
            "Can't modify owner %s : %s\n", path,
            strerror(errno));
        exit(EXIT_FAILURE);
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
    const char s[2] = ":";
    char *token;
    gid_t gid = -1;

    // on prend l'utilisateur
    token = strtok(argv[optind], s);
    if (isnumber(token)){
        uid = atoi(token);
    }
    else{
        pwd = getpwnam(token);      //On essaye d'avoir l'UID par le username
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
            if (isnumber(token)) {
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
        chownElem(argv[i], options, uid, gid);
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