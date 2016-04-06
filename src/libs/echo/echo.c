#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "../../LIB.h"

int kecho(int argc, char * argv[]) {
    int opt = 0;
    bool opt_n = false; // pour savoir si on a mis l'option -n

    if (argc < 2) { //echo sans argument
        printf("\n");
        return 0;
    }

    char c;
    while((c = getopt(argc, argv, "ne")) != -1) {
        opt++;
        switch(c) {
            case 'e': // Fait de base
                break;
            case 'n':
                opt_n = true;
                break;
            case '?': // option non reconnue
                exit(EXIT_FAILURE);
        }
    }

    int i;
    for (i = opt+1; i < argc; i++) {
        printf("%s", argv[i]);
        if (i < argc - 1) {
            printf(" ");
        }
    }

    if (!opt_n) {
        printf("\n");
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
    enregisterCommande("echo", kecho);
}
