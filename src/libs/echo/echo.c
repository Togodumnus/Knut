#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "../../LIB.h"

int kecho(int argc, char * argv[]) {
    int opt = 0;
    int opt_n = 0; // pour savoir si on a mis l'option -n
    if (argc < 2) {
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
                opt_n = 1;
                break;
            case '?': // option pas reconnu
                exit(-1);
        }
    }
    int i;
    for (i=opt+1;i<argc;i++) {
        printf("%s ", argv[i]);
    }
    if (!opt_n) printf("\n");
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
