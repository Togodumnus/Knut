#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "../../DEBUG.h"
#include "../../LIB.h"

/**
 * kecho_e
 *
 * echo avec option -e (traitement des \n, \t etc...)
 *
 * @param  {int}      opt    Le nombre d'option
 * @param  {int}      argc   Le nombre d'arguments
 * @param  {char[] *} argv   Les arguments d'entrées
 */
void kecho_e(int opt, int argc, char * argv[]) {

    DEBUG("option e");

    int i;
    for (i = opt; i < argc; i++) { //pour chaque argument

        //parcours caractère par caractère
        char c;
        int j = 0;
        int l = strlen(argv[i]);
        while (j < l) {
            c = argv[i][j];
            if (c == '\\') {
                j++;
                c = argv[i][j];
                switch (c) { // affichage du code héxa
                    case 'a':
                        printf("%c", 0x07);
                        break;
                    case 'b':
                        printf("%c", 0x08);
                        break;
                    case 'f':
                        printf("%c", 0x0C);
                        break;
                    case 'n':
                        printf("%c", 0x0A);
                        break;
                    case 'r':
                        printf("%c", 0x0D);
                        break;
                    case 't':
                        printf("%c", 0x09);
                        break;
                    case 'v':
                        printf("%c", 0x0B);
                        break;
                    case '\\':
                        printf("%c", 0x5C);
                        break;
                    case '\'':
                        printf("%c", 0x27);
                        break;
                    case '"':
                        printf("%c", 0x22);
                        break;
                    case '?':
                        printf("%c", 0x3F);
                        break;
                    default: // sinon on affiche
                        printf("\\%c", c);
                }
            } else {
                printf("%c", c);
            }

            j++;
        }

        if (i < argc - 1) {
            printf(" ");
        }

        j = 0;
    }
}

/**
 * kecho
 *
 * @param  {int}      argc   Le nombre d'arguments
 * @param  {char *[]} argv   Les arguments d'entrées
 */
int kecho(int argc, char * argv[]) {
    bool nFlag = false; // pour savoir si on a mis l'option -n
    bool eFlag = false;

    if (argc < 2) { //echo sans argument : saut de ligne
        printf("\n");
        return 0;
    }

    char c;
    while((c = getopt(argc, argv, "ne")) != -1) {
        switch(c) {
            case 'e':
                eFlag = true;
                break;
            case 'n':
                nFlag = true;
                break;
            case '?': // option reconnue
            default:
                perror("Unknow option");
                exit(EXIT_FAILURE);
        }
    }

    // echo avec -e
    if (eFlag) {
        kecho_e(optind, argc, argv);
    } else {

        DEBUG("no -e option");

        // echo sans -e
        // on affiche les arguments
        int i;
        for (i=optind; i<argc; i++) {
            printf("%s ", argv[i]);
        }

    }

    if (!nFlag) printf("\n");

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
