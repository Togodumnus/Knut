#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "../../LIB.h"
#include "../../DEBUG.h"

const int vFlag = 0x1;
const int pFlag = 0x2;

#ifdef __APPLE__
mode_t getumask(void) {
    mode_t mask = umask( 0 );
    umask(mask);
    return mask;
}
#endif

/**
 * octal_decimal
 *
 * Convertit un nombre decimal en octal
 *
 * @param  {int}  n    Le nombre à convertir
 */
int octal_decimal(int n) {
    int decimal=0, i=0, rem;
    while (n!=0)
    {
        rem = n%10;
        n/=10;
        decimal += rem * pow(8,i);
        ++i;
    }
    return decimal;
}

/**
 * kmkdir_v
 *
 * Option 'v' de mkdir : affiche un message
 *
 * @param  {char *}  dirname    Le nom du répertoire créé
*/
int kmkdir_v(char * dirname) {
    printf("mkdir: created directory %s\n", dirname);
    return 0;
}

/**
 * kmkdir_p
 *
 * Compatible avec l'option 'p' de mkdir :
 * cré les répertoires parents nécessaire
 *
 * @param  {char *}   path           Le chemin du répertoire à créé
 * @param  {int}      permissions    Les permissions du répertoire créé
 * @param  {bool}     verbose        Verbose flag
 */
int kmkdir_p(char *path, int permissions, bool verbose) {

    DEBUG("will mkdir %s", path);

    char *ch = path;
    int len = strlen(path);

    if (*path == '/') {
        ch++;
    }

    if (path[len - 1] == '/') {
        path[len - 1] = '\0';
    }

    for (; *ch; ch++) {
        if (*ch == '/') {
            *ch = '\0';
            DEBUG("mkdir %s", path);
            if (mkdir(path, permissions) < 0) {
                if (errno == EEXIST) {
                    continue;
                } else {
                    perror("Mkdir error");
                    return -1;
                }
            } else if (verbose) {
                printf("Create %s dir\n", path);
            }
            *ch = '/';
        }
    }

    DEBUG("mkdir %s", path);
    if (mkdir(path, permissions) < 0) {
        perror("Mkdir error");
        return -1;
    }

    return 0;
}

/**
 * kmkdir
 *
 * Fonction principale pour traiter les options
 *
 * @param  {int}       argc    Le nombre d'arguments d'entrée
 * @param  {char[] *}  argv    Les arguments d'entrée
 */
int kmkdir(int argc, char * argv[]) {

    if (argc < 2) {
        printf("kmkdir : missing operand\n");
        exit(EXIT_FAILURE);
    }

    int c;
    int flag = 0; //verbose and p flags
    int nbParams = 1;

    //on applique le mask des permissions
    mode_t permissions = 0777 & ~getumask();

    while ((c = getopt(argc, argv, "pm:v")) != -1) {
        switch(c) {
            case 'm':
                DEBUG("m option");
                permissions = octal_decimal(atoi(optarg));
                nbParams += 2;
                break;
            case 'v':
                DEBUG("v option");
                flag |= vFlag;
                nbParams++;
                break;
            case 'p':
                DEBUG("p option");
                flag |= pFlag;
                nbParams++;
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    if (permissions < 0 || permissions > 0777) {
        perror("Wrong permissions");
    }

    char *dir = argv[nbParams];

    do {
        if ((flag & pFlag) == pFlag) {
            if (kmkdir_p(argv[nbParams], permissions,
                        (flag & vFlag) == vFlag) == -1) {
                return EXIT_FAILURE;
            }
        } else {
            if (mkdir(argv[nbParams], permissions) == -1) {
                perror("Mkdir error");
                return EXIT_FAILURE;
            }
        }

        dir = argv[nbParams++];

    } while (nbParams < argc);

    return 0;
}

/**
 * Init
 *
 * S'enregistre dans le shell dans le cas d'un chargement de la librairie
 * dynamique
 */
void Init(EnregisterCommande enregisterCommande) {
    enregisterCommande("mkdir", kmkdir);
}
