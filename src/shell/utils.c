#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#ifdef __APPLE__
    #include <limits.h>
#else
    #include <linux/limits.h>
#endif

#include "utils.h"
#include "../DEBUG.h"

extern char *optchar;

/**
 * fileExtension
 *
 * @param  {char *}  file       Le fichier dont il faut trouver l'extension
 * @return {char *}             NULL si pas d'extension trouvée
 *                              Un pointeur sur le premier caractère de
 *                              l'extension sinon
 */
char *fileExtension(char *file) {

    //on cherche le point dans file
    char *point = strrchr(file, '.');

    //si pas de point ou point au début
    if (point == NULL || point == file) {
        return NULL;
    }

    //on retourne le pointeur sur le premier caractère de l'extension
    return point + 1;
}

/**
 * updatePATH
 *
 * Ajout d'un élément au $PATH (devant)
 * ie. utile pour ajouter le répertoir vers nos exécutable
 *
 * @see main.c
 * @param  {char *} prefix      La chaîne a ajouter devant le $PATH existant
 */
void updatePATH(const char* prefix) {

    char newPATH[PATH_MAX];
    sprintf(newPATH, "PATH=%s:%s", prefix, getenv("PATH"));

    putenv(newPATH);
}

/***
 * readArgs
 *
 * Un petit helper pour lire les arguments passé aux script
 *
 * Deux modes d'exécutions :
 * - normal (./bin/knutShell [-m <mode>]
 * - distant (./bin/knutShell connect [<addr>] <port>
 *
 * @param  {int}                    argc
 * @param  {char *}                 argv
 * @param  {enum execution_mode *}  mode    Le mode à lire après -m
 * @param  {char **}                addr    L'adresse de la machine distante
 *                                          à remplir si connect
 * @param  {int *}                  port    Le port de la machine distante à
 *                                          remplir si connect
 */
void readArgs(int argc, char *argv[],
        enum execution_mode *mode, char **addr, int *port) {
    char option = 0;

    // ./bin/KnutShell connect [addr] <port>
    if (argc == 3 && strcmp(argv[1], "connect") == 0) {
        *port = atoi(argv[2]);
    } else if (argc == 4 && strcmp(argv[1], "connect") == 0) {
        *addr = argv[2];
        *port = atoi(argv[3]);
    } else { // ./bin/KnutShell [-m <mode>]

        while ((option = getopt(argc, argv,"m:")) != -1) {
            if (option == 'm') {
                if (strcmp("exec", optarg) == 0) {
                    *mode = EXECUTABLE_MODE;
                } else if (strcmp("static", optarg) == 0) {
                    *mode = LIB_STATIC_MODE;
                } else if (strcmp("dynamic", optarg) == 0) {
                    *mode = LIB_DYNAMIC_MODE;
                }
            }
        }
    }

}

/**
 * printPrompt
 *
 * Affiche le prompt avant une commande
 */
void printPrompt(int fd) {
    dprintf(fd, GREEN "Toto" WHITE " @ " YELLOW "KnutShell\n" END);
    dprintf(fd, YELLOW "> " END);

    char eot = 4; //EOT : end of transmission
    if (write(fd, &eot, sizeof(char)) == -1) {
        perror("Can't end transmission with file descriptor");
        exit(EXIT_FAILURE);
    }
}
