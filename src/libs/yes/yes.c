#include <stdio.h>

#include "../../LIB.h"

int yesLib(int argc, char *argv[]) {
    if (argc > 1) {
        while (42) {
            printf("%s\n", argv[1]);
        }
    } else {
        while (42) {
            printf("yes\n");
        }
    }

}

/**
 * Init
 *
 * S'enregistre dans le shell dans le cas d'un chargement de la librairie
 * dynamique
 */
void Init(EnregisterCommande enregisterCommande) {
    enregisterCommande("yes", yesLib);
}
