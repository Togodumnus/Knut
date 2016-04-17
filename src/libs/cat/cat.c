#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>

#include "../../DEBUG.h"

const int N_FLAG    = 0x1;
const int E_FLAG    = 0x2;
const int B_FLAG    = 0x4;

void catLib(FILE *file, int option) {
    int nbligne = 0;
    char old = '\n';

    char c;
    while ((c = fgetc(file)) && !feof(file)) {

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

int usageCat() {

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
    optind = 1;

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
                return usageCat();
                break;
        }
    }

    for (int i = optind; i < argc; i++){

        FILE *file;
        file = fopen(argv[i], "r");
        if (file != NULL) {
            catLib(file, option);
            fclose(file);
        } else{
            printf("Erreur avec les fichiers\n");
            return 1;
        }
    }

    if (optind == argc) { //appel de cat sans fichier = on écoute stdin
        catLib(stdin, option);
    }

    return 0;
}

