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
#include <signal.h>

#include "utils.h"
#include "../DEBUG.h"

const int PATH_LENGTH = 2048;

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

    char newPATH[PATH_LENGTH];

    char *pwd = NULL;
    size_t n = 0;
    pwd = (char *) getcwd(pwd, n);
    if (pwd == NULL) {
        perror("get_current_dir_name failure");
        exit(EXIT_FAILURE);
    }

    sprintf(newPATH, "%s/%s:%s", pwd, prefix, getenv("PATH"));

    DEBUG("new PATH=%s", newPATH);
    setenv("PATH", newPATH, 1);

    free(pwd);
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

void SIGUSR1_handler_exit(int sig) {
    DEBUG("SIGUSR1 handler exit pid=%d", getpid());
    printf("Bye ! \n");
    fflush(stdout);
    fflush(stderr);
    exit(EXIT_SUCCESS);
}

void SIGINT_handler_nothing(int sig) {
    DEBUG("SIGINT handler nothing pid=%d", getpid());
}

void SIGINT_handler_message(int sig) {
    DEBUG("SIGINT handler message pid=%d", getpid());
    printf("\ntype ^D to exit\n");
    printPrompt(fileno(stdin));
}

void setSigHandler(void (*handler)(), int sig) {

    if (handler == NULL) { //reset sur le handler par défaut
        signal(sig, SIG_DFL);
    } else {
        struct sigaction act, oact;
        sigaction(sig, NULL, &oact); //copie du handler actuel
        act.sa_handler = handler;
        act.sa_mask  = oact.sa_mask;
        act.sa_flags = SA_RESTART;
        sigaction(sig, &act, NULL);
    }

}
