#pragma once

#include <stdbool.h>

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
     * andBefore
     *
     * Vaut true si l'action était précédée d'un ET logique &&
     */
    bool andBefore;

    /**
     * orBefore
     *
     * Vaut true si l'action était précédée d'un OU logique ||
     */
    bool orBefore;

    /**
     * pipe
     *
     * Vaut true si il faut piper le stdout précédent en entré de l'action
     */
    bool pipe;

};

typedef struct Action Action;

