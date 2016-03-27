#pragma once

#include <stdbool.h>

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

    char *fromFile;     //par exemple cat > toto.txt
    char *toFile;       //par exemple cat < toto.txt
    bool appendFile;    //true si >>, false si > <
};

typedef struct Command Command;
