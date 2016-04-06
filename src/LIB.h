#pragma once

/**
 * EnregisterCommande
 *
 * Un type pour les pointeur de fontion qui pointent sur la fonction
 * d'enregistrement de commandes du shell
 */
typedef void (*EnregisterCommande)(
    char *name,
    int (*pFunc)(int argc, char *argv[])
);
