#pragma once

/**
 * CommandType
 */
enum CommandType {
    SIMPLE, //la commande est un path vers un exécutable, ex: "./bin/libs/ls"
            //ou un nom de commande, ex: "ls"
    COMPLEX //la commande est une action, ex: "echo hello && ls -l"
};

typedef enum CommandType CommandType;

struct Command {

    CommandType type;

    /**
     * cmd
     *
     * La commande à lancer
     */
    char *cmd;

    int argc;
    char **argv;
};

typedef struct Command Command;
