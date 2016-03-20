#ifndef __APPLE__
    #define _GNU_SOURCE
#endif
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "process.h"
#include "Action.h"
#include "Command.h"

const int PIPE_READ  = 0;
const int PIPE_WRITE = 1;

//TODO remove
char *argvTest1[] = {"rm", "/bin", NULL};
char *argvTest2[] = {"yes", NULL};


/**
 * extractionActions
 *
 * Extractions des actions à effectuer d'une chaîne de caractère entrée par
 * l'utilisateur
 *
 * @example : rm /bin || (echo error && ls -l /)
 *            |------|   |---------------------|
 *             action           action
 *
 * @param   {char *}        str     Un pointeur sur la chaîne d'entrée
 * @param   {Action ***}    actions Un pointeur un futur tableau de pointeurs
 *                                  sur les actions.
 * @param   {int *}         actc    Un pointeur sur un int qui va stocker le
 *                                  nombre d'actions
 */
void extractionActions(char *str, Action ***actions, int *actc) {

    //TODO : faire un automate à état fini pour lire les actions
    //séparées par |, ||, && ou ;
    //pour l'instant on crée une action fake

    Action *action = (Action *) malloc(sizeof(Action));
    action->cmd = "/bin/ls /bin";
    action->andBefore = false;
    action->orBefore  = false;
    action->pipe      = true;

    Action *action2 = (Action *) malloc(sizeof(Action));
    action2->cmd = "yes";
    action2->andBefore = true;
    action2->orBefore  = false;
    action->pipe       = true;

    *actions = (Action **) malloc(2 * sizeof(Action*));
    (*actions)[0] = action;
    (*actions)[1] = action2;

    *actc = 2;
}

Command lectureAction(Action *action) {
    //TODO : faire un automate qui lit action->cmd
    //pour l'instant on crée une commande fake

    Command command;
    if (!action->andBefore) {
        command.type = EXECUTABLE;
        command.cmd = "rm";
        command.argc = 2;
        command.argv = argvTest1;
    } else {
        command.type = LIBRARY;
        command.cmd = "yes";
        command.argc = 1;
        command.argv = argvTest2;
    }

    return command;
}

int process(char *str) {

    //TODO remove
    FILE *tmp = fopen("test.txt", "w+");
    //dprintf(fileno(tmp), "[child %d] \tok\n", i);

    //Extraction des actions à effectuer de l'input
    Action **actions = NULL;
    int actc = 0; //nombre d'actions
    extractionActions(str, &actions, &actc);
    printf("[parent]\t%d actions to do\n", actc);

    int status = 0; //status code of execution

    //let's create pipes to chain stdin and stdout
    int *pstdin  = (int *) malloc(2 * sizeof(int)),
        *pstdout = (int *) malloc(2 * sizeof(int));

    if (pstdin == NULL || pstdout == NULL) {
        perror("Error pipes allocation");
        exit(1);
    }

    //On exécute et chaîne chaque action

    for (int i = 0; i < actc; i++) {

        Action *action = actions[i];
        printf("[parent]\taction %d (%p): %s\n", i, action, action->cmd);
        Command cmd = lectureAction(action);

        if (i > 0) {
            if (i > 1) {
                close(pstdin[0]);
                close(pstdin[1]);
            }
            pstdin[0] = pstdout[0];
            pstdin[1] = pstdout[1];
        } else {
            //la première action est branchée sur stdin
            pstdin[PIPE_READ] = STDIN_FILENO;
            pstdin[PIPE_WRITE] = -1;
        }

        if (i < actc - 1) {
            //create a pipe for stdout
            if (pipe(pstdout) == -1) {
                perror("Error pipe creation");
                exit(2);
            }
        } else {
            //on attache just pstdout sur stdout
            pstdout[PIPE_WRITE] = STDOUT_FILENO;
            pstdout[PIPE_READ] = -1;
        }

        int pid_child = fork();

        if (pid_child == -1) {
            perror("Can't fork");
            exit(3);
        }

        if (pid_child == 0) { //child

            printf("[child %d] \t%d start\n",i, getpid());

            //replace stdin
            if (pstdin[PIPE_READ] != STDIN_FILENO) {
                dup2(pstdin[PIPE_READ],   STDIN_FILENO);
                close(pstdin[PIPE_READ]);
            }
            //replace stdout
            if (pstdout[PIPE_WRITE] != STDOUT_FILENO) {
                dup2(pstdout[PIPE_WRITE], STDOUT_FILENO);
                close(pstdout[PIPE_WRITE]);
            }

            switch (cmd.type) {
                case EXECUTABLE:
                    dprintf(fileno(tmp), "[child %d] EXECUTABLE %s\n", i, action->cmd);
                    exit(0);
                    //exit(1);
                    break;
                case LIBRARY:
                    dprintf(fileno(tmp), "[child %d] LIBRARY %s\n", i, action->cmd);
                    exit(0);
                    //exit(...);
                    break;
                case ACTION:
                    dprintf(fileno(tmp), "[child %d] ACTION %s\n", i, action->cmd);
                    exit(process(action->cmd));
                    break;
                default:
                    perror("Unknown action type");
                    exit(1);
                    break;
            }

        } else {

            waitpid(pid_child, &status, 0);
            printf("[parent]\tChild %s %d exited with code %d\n",
                    action->cmd, pid_child, status);
        }

        free(action);
    }

    free(pstdin);
    free(pstdout);
    free(actions);

    return status;

}
