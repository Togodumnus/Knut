#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#ifdef __APPLE__
    #include <limits.h>
#else
    #include <linux/limits.h>
#endif

#include "Command.h"
#include "../DEBUG.h"

/**
 * cd
 *
 * @param  {char *} arg    Pointeur vers l'argument du cd ou NULL
 *
 * @retrun {int}        Le code d'erreur ou 0
 */
int cd(const char *arg) {
    char path[PATH_MAX];

    if (arg == NULL) { //si cd seul
        path = getenv("HOME");
    } else if (*arg == '~') {
        //On remplace le ~ par le chemin $HOME
        sprintf(path, "%s%s", getenv("HOME"), arg + 1);
    } else {
        strcpy(path, arg);
    }
    DEBUG("cd %s", path);

    if (chdir(path) == -1) {
        if (errno == EACCES) {
            printf("Can't access %s\n", path);
        } else if (errno == ENOENT) {
            printf("No such directory %s\n", path);
        } else if (errno == ENOTDIR) {
            printf("Not a directory %s\n", path);
        }
        return errno;
    } else {
        return 0;
    }
}

/**
 * shellCommand
 *
 * @param  {Command *}  cmd     La commande à exécuter
 *
 * @return {int}        Le code de retour ou -1 si pas de commande shell trouvé
 *                      pour cmd
 */
int shellCommand(Command *cmd) {

    if (strcmp(cmd->cmd, "cd") == 0) {
        return cd(cmd->argv[1]);
    }

    return -1;
}
