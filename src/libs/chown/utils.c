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

const int F_REC   = 1<<1;   //-r, -R
struct  group *gr, *getgrnam(), *getgrgid();
struct passwd *pwd;

void chElem(char *path, int options, uid_t uid, gid_t gid);

/**
 * chDirContent
 *
 * Parcours du contenu d'un répertoire pour ch
 *
 * @param  {char *}     path
 * @param  {uid_t}      uid
 * @param  {gid_t}      gid
 */
void chDirContent(char *path, int options, uid_t uid, gid_t gid){
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

            chElem(child_path, options, uid, gid); //on supprime l'élément
        }
    }

    if (closedir(dirp_src) != 0) {
        perror("Closedir error");
        exit(EXIT_FAILURE);
    }
}

/**
 * chElem
 *
 * Modification des permissions de path
 *
 * @param  {char *}     path
 * @param  {uid_t}      uid
 * @param  {gid_t}      gid
 */
void chElem(char *path, int options, uid_t uid, gid_t gid){
    struct stat st;
    if (stat(path, &st) == -1) {
        if (errno == ENOENT) {
            perror("No such file");
            exit(EXIT_FAILURE);
        }
    }

    if (S_ISDIR(st.st_mode)) {

        //On regarde si on a l'option -r
        if ((options & F_REC) == F_REC) {
            DEBUG("ch %s content", path);
            //modification des droits du contenu
            chDirContent(path, options, uid, gid);
        }

    }

    DEBUG("ch %s", path);
    if (chown(path, uid, gid) == -1){
        fprintf(stderr,
            "Can't modify owner/group %s : %s\n", path,
            strerror(errno));
        exit(EXIT_FAILURE);
    }
}