#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/types.h>
#ifdef __APPLE__
    #include <limits.h>
#else
    #include <linux/limits.h>
#endif

#include "../DEBUG.h"
#include "utils.h"

#include "libs.h"

/**
 * Commande
 *
 * Représente une commande enregistrée par une librairie
 */
struct Commande {
    char *name;                        //par exemple, "ls"
    CommandeFonction commandeFonction; //par exemple, le main de la lib ls
};
typedef struct Commande Commande;

/**
 * commandes
 *
 * Stocke l'ensemble des commande disponibles (enregistrées)
 */
struct Commandes {
    int size;           //le nombre de commandes dispo
    Commande **liste;    //le tableau des commandes

} commandes;

/**
 * enregisterCommande
 *
 * @param   {char *}            name                Nom de la commande
 * @param   {CommandeFonction}  commandeFonction    Le pointeur vers la fonction
 *                                                  d'entrée sur la commande
 */
void enregisterCommande(char *name, CommandeFonction commandeFonction) {

    DEBUG("Enregistrement commande : %s", name);

    Commande *newCommande = (Commande *) malloc(sizeof(Commande));

    if (newCommande == NULL) {
        perror("Malloc error");
        exit(EXIT_FAILURE);
    }
    newCommande->name = name;
    newCommande->commandeFonction = commandeFonction;

    Commande **newList = (Commande **) realloc(
            commandes.liste,
            (commandes.size + 1) * sizeof(Commande *)
    );

    if (newList == NULL) {
        perror("Malloc error");
        exit(EXIT_FAILURE);
    }
    commandes.liste = newList;
    commandes.liste[commandes.size] = newCommande;
    commandes.size++;
}

/**
 * showCommandes
 *
 * Print les commandes disponibles
 */
void showCommandes() {
    printf("Commands : [");
    for (int i = 0; i < commandes.size; i++) {
        printf("%s", commandes.liste[i]->name);
        if (i < commandes.size-1) {
            printf(", ");
        } else {
            printf("]\n");
        }
    }
}

/**
 * loadLib
 *
 * Charge la librairie et lance sa fonction init
 *
 * @param  {char *} dir     Le dossier où chercher la lib
 * @param  {char *} file    La lib
 */
void loadLib(const char *dir, const char *file) {

    void *libFile;
    InitFonction pInit;

    DEBUG("loadLib %s%s", dir, file);

    char *path = (char *) malloc(PATH_MAX * sizeof(char));
    sprintf(path, "%s%s", dir, file);

    if ((libFile = dlopen(path, RTLD_LAZY)) == NULL) {
        perror("lib introuvable");
        exit(EXIT_FAILURE);
    }

    if ((pInit = (InitFonction) dlsym(libFile, "Init")) == NULL) {
        perror("fonction introuvable dans la lib");
        exit(EXIT_FAILURE);
    }

    (pInit)(enregisterCommande); //on lance l'init de la lib

    free(path);
}


/**
 * loadDynamicLibs
 *
 * Charge les librairies de `libs` et appelle leur fonctions Init
 *
 * @param {char *}  libdir      Le dossier où il faut chercher les libs
 */
void loadDynamicLibs(const char *libdir) {

    DEBUG("loading dynamic libs");

    DIR *directory;

    directory = opendir(libdir); //on ouvre le dossier
    if (directory != NULL) {
        struct dirent *file;
        const char *EXTENSION = "so";

        while ((file = readdir(directory))) { //on parcours tous les fichiers
            //on charge les fichiers .so uniquement
            char *file_extension = fileExtension(file->d_name);
            if (file_extension != NULL
                    && strcmp(file_extension, EXTENSION) == 0) {
                DEBUG("loadLib %s", file->d_name);
                loadLib(libdir, file->d_name);
            }
        }
        closedir(directory);
    }
    else {
        perror("Impossible de charger les librairies à partir de $libdir");
    }
}

/**
 * findCommande
 *
 * Recherche si la commande cmd fait partie des librairies qui sont disponibles
 * et retourne le pointeur sa fonction d'entrée si possible
 *
 * @param   {char *}    cmd     La commande qu'il faut trouver
 *
 * @return  {CommandeFonction}  Le pointeur vers la fonction ou NULL si la lib
 *                              n'est pas trouvée
 */
CommandeFonction findCommande(char *cmd) {

    for (int i = 0; i < commandes.size; i++) {
        if (strcmp(commandes.liste[i]->name, cmd) == 0) {
            return commandes.liste[i]->commandeFonction;
        }
    }

    return NULL;
}
