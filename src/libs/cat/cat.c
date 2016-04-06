#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>

#include "../../LIB.h"

const int N_FLAG    = 0x1;
const int E_FLAG    = 0x2;
const int B_FLAG    = 0x4;

void catLib(FILE *file, int option) {
    int nbligne = 0;

    char old = '\n',
         c;

    while ((c = fgetc(file)) != EOF) {

        if (old == '\n') {
            if ((option & N_FLAG) == N_FLAG
                    || (((option & B_FLAG) == B_FLAG) && c != '\n')) {
                nbligne++;
                printf("   %d ", nbligne);
            }
        }

        if ((option & E_FLAG) == E_FLAG) {
            if (c == '\n') {
                if (putchar('$') == EOF) {
                    perror("Write error");
                    exit(EXIT_FAILURE);
                }
            } else if (!isprint(c)) {
                printf("M-");
            } else if (iscntrl(c)) {
                printf("^X");
            }

        }

        if (putchar(c) == EOF){
            perror("Write error");
            exit(EXIT_FAILURE);
        }


        old = c;
    }

    putchar('\n');
}

int catLib_option(int argc, char *argv[], int option){
    FILE *file;

    int nbFile = 0;
    for (int i = 1; i < argc; i++){
        //si on est sur une option, on passe
        if (*(argv[i]) == '-') {
            continue;
        }

        file = fopen(argv[i], "r");
        if (file != NULL) {
            catLib(file, option);
        } else{
            printf("Erreur avec les fichiers\n");
            return 1;
        }
        nbFile++;
    }

    if (nbFile == 0) { //appel de cat sans fichier = on écoute stdin
        catLib(stdin, option);
    }
    return 0;
}

int usage() {

    printf("\
Knut cat \n\
\n\
usage: cat [-enb] [file]\n\
\n\
Arguments: \n\
\t-n\tDisplay line number.\n\
\t-b\tDisplay line number for non-blank lines.\n\
\t-e\tDisplay non-printing characters (simple version). \n\
\t\tEnd of line are $, controls are ^?, non-printing are M-.\n\
");

    return 0;
}

int kCatLib(int argc,char *argv[]){
    int opt = 0;
    int option = 0;

    while((opt = getopt(argc, argv, "enb")) != -1) {
        switch (opt) {
            case 'e':
                option |= E_FLAG;
                break;
            case 'n':
                option |= N_FLAG;
                break;
            case 'b':
                option |= B_FLAG;
                break;

            default:
                return usage();
                break;
        }
    }
    return catLib_option(argc, argv, option);
}


/**
 * Init
 *
 * S'enregistre dans le shell dans le cas d'un chargement de la librairie
 * dynamique
 */
void Init(EnregisterCommande enregisterCommande) {
    enregisterCommande("cat", kCatLib);
}
