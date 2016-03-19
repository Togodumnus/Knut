#ifndef __APPLE__
    #define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>

#include "front.h"

/**
 * readLine
 *
 * Lit une ligne sur stdin
 *
 * @param {char **}    Un pointer vers le pointer à fournir à getline
 * @return {int}       -1 si erreur, 0 sinon
 */
int readLine(char **line) {
    size_t size;
    return getline(line, &size, stdin);
}

/**
 * printPrompt
 *
 * Affiche le prompt avant une commande
 */
void printPrompt() {
    printf("Toto @ KnutShell\n"); //TODO
    printf("> ");
}
