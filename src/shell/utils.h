#pragma once

enum execution_mode {
    EXECUTABLE_MODE,  //utilisation de nos commandes sous forme d'éxécutables
    LIB_STATIC_MODE,  // ... de librairies statiques
    LIB_DYNAMIC_MODE  // ... de librairies dynamiques
};

char *fileExtension(char *file);

void updatePATH(const char* prefix);

void readArgs(int argc, char *argv[],
        enum execution_mode *mode, char **addr, int *connect);

