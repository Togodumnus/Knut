#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#ifdef __APPLE__
    #include <limits.h>
#else
    #include <linux/limits.h>
#endif

/**
 * cd
 *
 * @param  {char *} arg    Pointeur vers l'argument du cd ou NULL
 */
void cd(const char *arg) {
    char *path = (char *) malloc(sizeof(char) * PATH_MAX);

    if (arg == NULL) { //si cd seul
        path = getenv("HOME");
    }

    //On remplace le ~ par le chemin $HOME
    if (*arg == '~') {
        sprintf(path, "%s%s", getenv("HOME"), arg + 1);
    }

    if (chdir(path) == -1) {
        perror("cd error");
    }

    free(path);
}

//TODO pass a Command here
void shellCommand(char *cmd, char *arg) {
    if (strcmp(cmd, "cd") == 0) {
        cd(arg);
    }
}
