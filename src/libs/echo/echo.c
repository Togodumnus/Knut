#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/**
 * kecho_e
 *
 * echo avec option -e (traitement des \n, \t etc...)
 *
 * @param  {int}      opt    Le nombre d'option
 * @param  {int}      argc   Le nombre d'arguments
 * @param  {char[] *} argv   Les arguments d'entrées
 */
int kecho_e(int opt, int argc, char * argv[]) {
    int i;
    int j = 0;
    char c;
    for (i=opt+1;i<argc;i++) {
        while (j < strlen(argv[i])) {
            c = argv[i][j];
            if (c == '\\') {
                j++;
                c = argv[i][j];
                switch (c) {
                    case 'a':
                        printf("%c",0x07);
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
                }
            }
            else 
                printf("%c", c);
            j++;
        }
    }
    return 0;
}

/**
 * kecho
 *
 * @param  {int}      argc   Le nombre d'arguments
 * @param  {char[] *} argv   Les arguments d'entrées
 */
int kecho(int argc, char * argv[]) {
    int opt = 0;
    int nFlag = 0; // pour savoir si on a mis l'option -n
    int eFlag = 0;
    if (argc<2) { 
        printf("\n");
        return 0;
    }
    char c;
    while((c = getopt(argc, argv, "ne")) != -1) {
        switch(c) {
            case 'e': // Fait de base 
                eFlag = 1;
                opt++;
                break;
            case 'n': 
                nFlag = 1;
                opt++;
                break;
            case '?': // option pas reconnu
                perror("Unknow option");
                exit(EXIT_FAILURE);
        }
    }
    // echo avec -e
    if (eFlag) {
        if (!kecho_e(opt, argc, argv)) {
            if (!nFlag) printf("\n");   
            return 0;
        }
    }
    // echo sans -e
    int i;
    for (i=opt+1;i<argc;i++) {
        printf("%s ", argv[i]);
    }
    if (!nFlag) printf("\n");   
    return 0;
}