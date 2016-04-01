#pragma once

#include <stdbool.h>

/**
 * ChainingType
 *
 * Définit les différents lien entre processus
 *
 * @example ls -ls | sort           PIPE
 * @example rm /bin || echo error   OR
 * @example rm /bin && echo ahaha   AND
 * @example ls; echo hello          COMMA
 */
enum ChainingType {
    CHAINING_COMMA,  //aucun lien entre deux process (séparation par ;)
    CHAINING_AND,    //ET logique (séparation par &&)
    CHAINING_OR,     //OU logique (séparation par ||)
    CHAINING_PIPE    //pipe des entrées / sorties (séparation par |)
};

typedef enum ChainingType ChainingType;

struct Action {

    /**
     * cmd
     *
     * Une commande à exécuter, peut-être une action
     *
     * @example "rm /bin"                   Une commande
     * @example "(echo hello && ls -l)"     Une autre action
     */
    char *cmd;

    /**
     * length
     *
     * La taille de Action.cmd
     */
    int length;

    ChainingType chainingType;

    /**
     * background
     *
     * true si l'action doit être détachée à l'éxécution
     * ie. si elle est suivie de `&`
     */
    bool background;
};

typedef struct Action Action;

