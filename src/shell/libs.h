#pragma once

#include "../LIB.h"

/**
 * CommandeFonction
 *
 * Un type pour les pointeurs de fonction vers les fonctions d'entrée des libs
 */
typedef int (*CommandeFonction)(int argc, char *argv[]);

/**
 * InitFonction
 *
 * Un type pour les pointeurs vers les fonction init des libs
 */
typedef void (*InitFonction)(EnregisterCommande);

void loadLib(const char *dir, const char *file);
void loadDynamicLibs(char *libdir);
void enregisterCommande(char *name, CommandeFonction commandeFonction);

CommandeFonction findCommande(char *cmd);
