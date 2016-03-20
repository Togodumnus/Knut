#pragma once

/**
 * CommandType
 */
enum CommandType {
    EXECUTABLE, //la commande est un path vers un exécutable, ex: "./bin/libs/ls"
    LIBRARY,    //la commande est un nom de libraire Knut, ex "ls"
    ACTION      //la commande est une action, ex: "echo hello && ls -l"
};

typedef enum CommandType CommandType;

struct Command {

    CommandType type;

    /**
     * cmd
     *
     * La commande à lancer :
     * - le path vers l'éxécutable si type == EXECUTABLE
     * - le nom de la library si type == LIBRARY
     * - la chaîne a analyser si type == ACTION
     */
    char *cmd;

    int argc;
    char **argv;
};

typedef struct Command Command;
