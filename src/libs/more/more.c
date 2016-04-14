#define _GNU_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

#include "../../LIB.h"
#include "../../DEBUG.h"

#include "more.h"

const int BUFFER = 4096;
const char *MORE = "----- More -----";

struct Term {
    int width;
    int height;
};

/**
 * configTerm
 *
 * Modifie le comportement du terminal. On ne veut pas attendre un \n ou un EOF
 * avant qu'un caractère entré sur stdin soit propagé
 * @see http://stackoverflow.com/a/1798833/2058840
 *
 * @return {struct termios}     La config initiale à reset à la fin
 */
struct termios configTerm() {
    struct termios oldt, newt;

    //copie de la config actuelle
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON); //ne pas attendre \n ou EOF
    newt.c_lflag &= ~ECHO; //ne pas afficher l'entrée stdin
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    return oldt;
}

/**
 * getTermSize
 *
 * Récupère la taille actuelle du terminal
 * @see http://stackoverflow.com/a/1022961/2058840
 *
 * @return {struct Term}
 */
struct Term getTermSize() {

    DEBUG("getTermSize");

    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    DEBUG("lines: %d", w.ws_row);
    DEBUG("columns: %d", w.ws_col);

    struct Term result = { w.ws_col, w.ws_row };
    return result;
}

/**
 * getFileSize
 *
 * @param  {char *}     file        path vers le fichier
 * @return {int}        Le poids en bytes du fichier
 *                      ou -1 si on ne peut pas le lire (stdin)
 */
int getFileSize(char *file) {

    struct stat st;
    if ((stat(file, &st) == 0)) {
        return (int) st.st_size;
    }

    return -1;
}

/**
 * prompt
 *
 * Affichage de la dernière ligne de more
 *
 * @param  {char *} msg     Le message à afficher
 *                          Si NULL, commande de nettoyage de la dernière ligne
 */
void prompt(const char *msg) {
    if (msg == NULL) {
        int width = 0;
        width = getTermSize().width;

        char spaces[width + 1];
        memset(spaces, ' ', width);
        spaces[width] = '\0';

        printf("\r%s", spaces);

        DEBUG("clean %d caracters", width);

    } else {
        printf(BLACK WHITE_BK "%s" END, msg);
    }
}


/**
 * moreStrean
 *
 * @param  {FILE *} stream      Stream à afficher
 * @param  {int}    weight      Taille du stream en bytes
 */
void moreStrean(FILE *stream, int weight) {

    DEBUG("moreStream");

    struct Term termInfos;
    termInfos = getTermSize();

    int line = 0;      //ligne courante
    bool end = false;

    int total = 0;

    //premier passage, on affiche sur toute la hauteur du terminal
    int k = termInfos.height - 1;

    while (!end) {

        //affichage

        int lineTmp = 0;   //line courante
        int lineIndex = 0; //index dans la ligne courante

        char c;
        do {

            if (lineIndex == termInfos.width) { //saut de ligne
                printf("\n");
                lineIndex = 0;
                lineTmp++;
            }

            c = fgetc(stream);

            if (c != EOF) {
                printf("%c", c);
                total++;
            }

            if (c == '\n') {
                lineTmp++;
                lineIndex = 0;
            }

            lineIndex++;

        } while (c != EOF && (lineTmp < k));

        if (c == EOF) {
            end = true;
        }

        line += k;

        //attente commande

        if (!end) {

            //Affichage "---- More ---"
            if (weight > -1) {
                char msg[1024];
                sprintf(msg, "%s (%d%%)", MORE, total * 100 / weight);
                prompt(msg);
            } else {
                prompt(MORE);
            }

            bool reading = true;

            //gestion du préfix numérique optionel devant une commande
            char prefix[1024];
            int prefixIndex = 0;

            char command;
            k = 1;
            do {
                command = getchar();

                if (command <= '9' && command >= '0') {
                    prefix[prefixIndex] = command;
                    prefixIndex++;
                } else {
                    DEBUG("%c stop reading", c);
                    reading = false;
                }
            } while (reading);

            if (command == ' ' || command == '\n' || command == 'z'
                    || command == 'd' || command == 4) { //4 = ^D
                if (prefixIndex > 0) {
                    prefix[prefixIndex] = '\0';
                    k = atoi(prefix);
                }
            } else if (command == 'q' || command == 'Q') {
                end = true;
            }

            if (command == 'd' || command == 4) {
                k *= termInfos.height;
            }

            prompt(NULL); //RAZ dernière ligne
            DEBUG("k=%d", k);
        }
    }

}

/**
 * usage
 */
void usage() {

    printf("\
Knut more \n\
\n\
usage: more file ...\n\
\n\
COMMANDS\n\
    SPACE, RETURN, z\tShow next line\n\
    d or ^D\t\tShow next page\n\
    q or Q\t\tquit\n\
");
}

/**
 * moreLib
 *
 * Fonction d'entrée
 *
 * @param  {int}        argc
 * @param  {char *[]}   argv
 */
int moreLib(int argc, char *argv[]) {

    int result = 0;
    struct termios initialTermConfig = configTerm();

    if (argc < 2) { //pas de fichier en paramètre
        usage();
    }

    //on affiche chaque fichier passé en paramètre
    for (int i = 1; i < argc; i++) {
        DEBUG("show file %s", argv[i]);

        FILE *file = fopen(argv[i], "r");

        if (file == NULL) {
            perror("Opening file error");
            exit(EXIT_FAILURE);
        }

        int weight = getFileSize(argv[i]);
        moreStrean(file, weight);

        fclose(file);

        if (i < argc - 1) {
            char msg[1024];
            sprintf(msg, "%s (Next file: %s)", MORE, argv[i + 1]);
            prompt(msg);
            getchar();
        }

    }

    //reset de la config du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &initialTermConfig);

    return result;
}

/**
 * Init
 *
 * S'enregistre dans le shell dans le cas d'un chargement de la librairie
 * dynamique
 */
void Init(EnregisterCommande enregisterCommande) {
    enregisterCommande("more", moreLib);
}
