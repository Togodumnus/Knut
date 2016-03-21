#pragma once

typedef int (*CommandeFonction)(int argc, char *argv[]);
void loadLib(char *lib, CommandeFonction *commandeFonction);
void loadLibs();
CommandeFonction findCommande(char *cmd);
