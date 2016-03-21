#pragma once

typedef int (*libFunc)(int argc, char *argv[]);
void loadLib(char *lib, int (**pFunc)(int argc, char *argv[]));
void loadLibs(char *commandes[], int (*commandesFonctions[])(int, char *[]));
