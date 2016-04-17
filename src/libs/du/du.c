#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#ifdef __APPLE__
#include <limits.h>
#else
#include <linux/limits.h>
#endif

#include "../../DEBUG.h"
#include "../../LIB.h"

/*
 * macros des unités tirées du code source de du (Apple)
 * @see http://opensource.apple.com/source/file_cmds/file_cmds-212/du/du.c
 */
#define KILO(n) (n)
#define MEGA(n) ((n) * (n))
#define GIGA(n) ((n) * (n) * (n))
#define TERA(n) ((n) * (n) * (n) * (n))
#define PETA(n) ((n) * (n) * (n) * (n) * (n))

#define KILO_SZ (KILO(1024ULL))
#define MEGA_SZ (MEGA(1024ULL))
#define GIGA_SZ (GIGA(1024ULL))
#define TERA_SZ (TERA(1024ULL))
#define PETA_SZ (PETA(1024ULL))

const int FLAG_a = 1;       //-a
const int FLAG_L = 1<<1;    //-L
const int FLAG_h = 1<<2;    //-h

int sizeDirContent(char *path, int options);

/**
 * isParent
 *
 * @return  {bool}  True si file = . ou ..
 */
bool isParent(char *file) {
    return strcmp(file, ".") == 0 || strcmp(file, "..") == 0;
}

/**
 * readHuman
 *
 * Convertit une taille en un affichage pour humain
 *
 * @param  {int}        size
 * @param  {char *}     result
 */
void readHuman(long int size, char *result) {

    if (size < KILO_SZ) {
        sprintf(result, "%ldB", size);
    } else if (size < MEGA_SZ) {
        sprintf(result, "%lluK", size / KILO_SZ);
    } else if (size < GIGA_SZ) {
        sprintf(result, "%lluM", size / MEGA_SZ);
    } else if (size < TERA_SZ) {
        sprintf(result, "%lluG", size / GIGA_SZ);
    } else if (size < GIGA_SZ) {
        sprintf(result, "%lluT", size / TERA_SZ);
    } else {
        sprintf(result, "%lluP", size / PETA_SZ);
    }

}

/**
 * sizeElement
 *
 * @param   {char *}    path     Chemin vers l'élément
 * @param   {int}       options
 *
 * @return  {long int}          Taille de l'élément en octet
 */
long int sizeElement(char *path, int options) {

    int size = 0;
    struct stat Stat;
    int (*statFunc)(const char *, struct stat *);

    DEBUG("sizeElement %s", path);

    if ((options & FLAG_L) == FLAG_L) {
        statFunc = &stat; //on suit les lien
    } else {
        statFunc = &lstat; //on ne suit pas les liens
    }

    if ((*statFunc)(path, &Stat) == 0) {

        //si dossier, appel récursif
        if (S_ISDIR(Stat.st_mode)) {
            DEBUG("[dir]");
            size +=  sizeDirContent(path, options);
        } else if (S_ISLNK(Stat.st_mode) || S_ISREG(Stat.st_mode)) {
            DEBUG("[file]");
            size = (int) Stat.st_blocks; //on ne veut pas la taille mais le
                                         //nombre de blocks occupés
            if ((options & FLAG_h) & FLAG_h) {
                size *= (int) Stat.st_blksize / 8;
                DEBUG("blksize = %d", (int) Stat.st_blksize);
            }
        } else {
            DEBUG("[?]");
        }

    } else {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    DEBUG("%s :\t%d", path, size);
    if ((options & FLAG_a) == FLAG_a || S_ISDIR(Stat.st_mode)) {

        if ((options & FLAG_h) & FLAG_h) { //on affiche l'info version humaine
            char msg[1024];
            readHuman(size, msg);
            printf("%s \t %s\n", msg, path);
        } else { //on affiche directement l'info
            printf("%d \t %s\n", size, path);
        }
    }

    return size;
}

/**
 * sizeDirContent
 *
 * Compute directory size with all its files and sub-directories
 * Calcule la taille d'un dossier (taille de tous ses sous-éléments)
 *
 * @param   {char *}    path    Chemin vers le dossier
 * @param   {int}       options
 *
 * @return  {int}               Taille du dossier en octets
 */
int sizeDirContent(char *path, int options) {

    DEBUG("sizeDirContent %s", path);

    long int size = 0;

    DIR *dirp;
    struct dirent *dptr;

    if ((dirp = opendir(path)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);;
    }

    while ((dptr = readdir(dirp)) != NULL) {
        DEBUG("element : %s", dptr->d_name);
        if (strcmp(dptr->d_name, ".") != 0 && strcmp(dptr->d_name, "..") != 0) {
            char elementPath[PATH_MAX];
            sprintf(elementPath, "%s/%s", path, dptr->d_name);
            size += sizeElement(elementPath, options);
        }
    }

    closedir(dirp);

    return size;

}

/**
 * usage
 */
void usage() {
    printf("\
Knut du\n\n\
usage: du [-Lach] file ...\n\
\n\
\t-L Follow symbolic links\n\
\t-a Show every entry in the file hierachy\n\
\t-c Display a total\n\
\t-h Display human-readable sizes\n\
");
}

/**
 * duLib
 *
 * Fonction d'entrée
 *
 * @param  {int}        argc
 * @param  {char *[]}   argv
 */
int duLib(int argc, char *argv[]) {
    char c;

    int options = 0;
    bool grandTotal = false;
    long int total = 0;

    while((c = getopt(argc, argv, "acLh")) != -1) {
        switch(c) {
            case 'a':
                options |= FLAG_a;
                break;
            case 'c':
                grandTotal = true;
                break;
            case 'L':
                options |= FLAG_L;
                break;
            case 'h':
                options |= FLAG_h;
                break;
            default:
            case '?': //option non reconnue
                usage();
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (argc > optind) { //pour chaque fichiers en argument
        for (int i = optind; i < argc; i ++) {
            DEBUG("DU %s", argv[i]);
            long int t = sizeElement(argv[i], options);
            total += t;
        }
    } else {
        long int t = sizeElement(".", options);
        total += t;
    }

    if (grandTotal) {
        if ((options & FLAG_h) == FLAG_h) {
            char msg[1024];
            readHuman(total, msg);
            printf("%s \t total\n", msg);
        } else {
            printf("%ld \t total\n", total);
        }
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
    enregisterCommande("du", duLib);
}
