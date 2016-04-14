#include <unistd.h>
#define _XOPEN_SOURCE 500
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#ifdef __APPLE__
#include <limits.h>
#else
#include <sys/limits.h>
#endif

#include "../../DEBUG.h"
#include "../../LIB.h"

#define PURPLE   "\033[1;35m"
#define CYAN     "\033[1;36m"
#define RED      "\033[38;5;124m"

const int FLAG_a = 1;    //-a option
const int FLAG_l = 1<<1; //-l option

char *monthName[] = {"janv.", "févr.", " mars", "avril", "  mai", " juin",
                     "juil.", " août", "sept.", " oct.", " nov.", " déc."};

/**
 * printColorFile
 *
 * Affiche le nom du fichier avec la couleur correspondante en fonction du type
 * de fichier
 *
 * @param  {mode_t}  mode   Le mode_t du fichier
 * @param  {char *}  mode   Le nom du fichier à affiche
*/
void printColorFile(mode_t mode, char * path) {
    if (S_ISDIR(mode)) {                    // repertoire
        printf(BLUE "%s/" END, path);
    } else if (S_ISLNK(mode)) {             // lien
        printf(CYAN "%s" END, path);
    } else if (S_ISSOCK(mode)) {            // socket
        printf(PURPLE "%s" END, path);
    } else if (S_ISCHR(mode) || S_ISBLK(mode) || S_ISFIFO(mode)) {
        // périphérique de caractères
        printf(YELLOW "%s" END, path);
    } else if ((S_IXUSR & mode) == S_IXUSR) {//exécutable
        printf(RED "%s" END, path);
    } else {
        printf("%s", path);
    }
}

/**
 * permission
 *
 * @param   {mode_t}    mode    Le mode du fichier à inspecter
 * @param   {char *}    perms   Le tableau de 10 caractère à remplir avec les
 *                              permissions
 *                              ex: rwxr--r--
 */
void permission(mode_t mode, char* perms) {

    perms[0] = ((S_IRUSR & mode) == S_IRUSR) ? 'r' : '-';
    perms[1] = ((S_IWUSR & mode) == S_IWUSR) ? 'w' : '-';
    perms[2] = ((S_IXUSR & mode) == S_IXUSR) ? 'x' : '-';

    perms[3] = ((S_IRGRP & mode) == S_IRGRP) ? 'r' : '-';
    perms[4] = ((S_IWGRP & mode) == S_IWGRP) ? 'w' : '-';
    perms[5] = ((S_IXGRP & mode) == S_IXGRP) ? 'x' : '-';

    perms[6] = ((S_IROTH & mode) == S_IROTH) ? 'r' : '-';
    perms[7] = ((S_IWOTH & mode) == S_IWOTH) ? 'w' : '-';
    perms[8] = ((S_IXOTH & mode) == S_IXOTH) ? 'x' : '-';

    perms[9] = '\0';

}

/**
 * type
 *
 * @param   {mode_t}    mode    Le mode du fichier à inspecter
 *
 * @return  {char}      Le type de fichier / dossier
 */
char type(mode_t mode) {
    if((S_IFDIR & mode) == S_IFDIR) {
        return 'd';
    } else if ((mode & S_IFLNK) == S_IFLNK) {
        return 'l';
    } else if ((mode & S_IFBLK) == S_IFBLK) {
        return 'b';
    } else if ((mode & S_IFIFO) == S_IFIFO) {
        return 'p';
    } else if ((mode & S_IFCHR) == S_IFCHR) {
        return 'c';
    } else if ((mode & S_IFSOCK) == S_IFSOCK) {
        return 's';
    } else {
        return '-';
    }
}

/**
 * isLink
 *
 * @return  {boo}   True if the file is a symlink
 */
bool isLink(mode_t mode) {
    return ((mode & S_IFLNK) == S_IFLNK);
}

/**
 * kls_file
 *
 * Print les infos sur le fichier filepath
 *
 * @param  {char *} dir
 * @param  {char *} filename
 * @param  {int}    options
 */
int kls_file(char *dir, char *filename, int options) {

    struct stat statls;
    bool optL = (options & FLAG_l) == FLAG_l;

    char filepath[PATH_MAX];
    sprintf(filepath, "%s/%s", dir, filename);

    if (stat(filepath, &statls) == -1) {
        perror("stat error");
        exit(EXIT_FAILURE);
    }

    if (optL) {
        char perms[10];
        permission(statls.st_mode, perms);

        struct tm *tmInfo;
        tmInfo = localtime(&statls.st_mtime);

        printf("%c", type(statls.st_mode));
        printf("%s", perms);
        printf(" ");
        printf("%3d", (unsigned int) statls.st_nlink);
        printf(" ");
        printf("%s", getpwuid(statls.st_uid)->pw_name);
        printf(" ");
        printf("%s", getgrgid(statls.st_gid)->gr_name);
        printf(" ");
        printf("%6d", (int) statls.st_size); // taille
        printf(" ");
        printf("%s %2d %2d:%02d\t",
            monthName[tmInfo->tm_mon],
            tmInfo->tm_mday,
            tmInfo->tm_hour,
            tmInfo->tm_min
        );
    }

    printColorFile(statls.st_mode, filename);

    if (optL) {
        if (isLink(statls.st_mode)){ //si symlink, ajout de la destination
            char buf[PATH_MAX];
            size_t len;
            if ((len = readlink(filepath, buf, sizeof(buf)-1)) != -1) {
                buf[len] = '\0';
            }
            printf(" -> %s", buf);
        }
        printf("\n");
    } else {
        printf("    ");
    }

    return 0;
}

/**
 * kls_full
 *
 * @param  {char *}     path
 * @param  {int}        options
*/
int kls_full(char *path, int options) {

    DIR *dirp;
    struct dirent *dptr;

    if ((dirp = opendir(path)) == NULL) {
        if (errno == ENOTDIR) {
            kls_file("", path, options);
            printf("\n");
            return 0;
        } else {
            perror("opendir error");
            exit(EXIT_FAILURE);
        }
    }

    while ((dptr = readdir(dirp))) {
        //prise en compte des fichiers caché si option
        if (dptr->d_name[0] != '.' || (options & FLAG_a) == FLAG_a) {
            kls_file(path, dptr->d_name, options);
        }
    }

    printf("\n");
    closedir(dirp);

    return 0;
}

/**
 * kls
 *
 * ls avec option -l et -a ou non (dépand de aFlag)
 *
 * @param  {int}       argc   Le nombre d'arguments
 * @param  {char[] *}  argv   Les arguments d'entrée
 * @param  {int}       aFlag  Option -a
*/
int kls(int argc, char *argv[]) {
    char c;

    int options = 0;

    while((c = getopt(argc, argv, "al")) != -1) {
        switch(c) {
            case 'a':
                options |= FLAG_a;
                break;
            case 'l':
                options |= FLAG_l;
                break;
            case '?':
                printf("kls: invalid option -- '%c'\n",c);
                break;
        }
    }

    if (optind == argc) {
        return kls_full(".", options);
    }

    for (int i = optind; i < argc; i++) {
        //On affiche le répertoire analysé si il y en a plusieurs
        if (optind != argc -1) {
            printf("%s:\n", argv[i]);
        }

        DEBUG("ls %s", argv[i]);
        kls_full(argv[i], options);
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
    enregisterCommande("ls", kls);
}
