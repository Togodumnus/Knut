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

void printPrompt(int fd);

void SIGUSR1_handler_exit(int sig);
void SIGINT_handler_nothing(int sig);
void SIGINT_handler_message(int sig);
void setSigHandler(void (*handler)(), int sig);
