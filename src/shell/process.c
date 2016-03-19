#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#include "process.h"
#include "Action.h"

const int PIPE_READ  = 0;
const int PIPE_WRITE = 1;

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
    action->cmd = "rm /bin";
    action->andBefore = false;
    action->orBefore  = false;
    action->pipe      = true;

    Action *action2 = (Action *) malloc(sizeof(Action));
    action2->cmd = "ls -l";
    action2->andBefore = true;
    action2->orBefore  = false;
    action->pipe       = true;

    *actions = (Action **) malloc(3 * sizeof(Action*));
    *actions[0] = action;
    *actions[1] = action2;

    *actc = 2;
}

Command lectureAction(...) {
    //TODO
}

int process(char *str) {

    //Extraction des actions à effectuer de l'input
    Action **actions = NULL;
    int actc = 0; //nombre d'actions
    extractionActions(str, &actions, &actc);

    int status = 0; //status code of execution

    //let's create pipes to chain stdin and stdout
    int *pstdin  = malloc(2 * sizeof(int)),
        *pstdout = malloc(2 * sizeof(int));

    if (pstdin == NULL || pstdout == NULL) {
        perror("Error pipes allocation");
        exit(1);
    }

    if (pipe(pstdout) == -1) {
        perror("Error pipe creation");
        exit(2);
    }

    //la première action est branchée sur stdin
    pstdin[PIPE_READ] = STDIN_FILENO;

    //On exécute et chaîne chaque action

    for (int i = 0; i < actc; i++) {

        Action *action = actions[i];
        Command cmd = lectureAction(action);

        printf("action : %s\n", action->cmd);

        if (i > 0) {
            close(pstdin[0]);
            close(pstdin[1]);
            pstdin[0] = pstdout[0];
            pstdin[1] = pstdout[1];
        }

        if (i < actc - 1) {
            //create a pipe for stdout
            if (pstdout == NULL) {
                perror("Error pipe allocation");
                exit(1);
            }
            if (pipe(pstdout) == -1) {
                perror("Error pipe creation");
                exit(2);
            }
        } else {
            //on attache just pstdout sur stdout
            pstdout[PIPE_WRITE] = STDOUT_FILENO;
        }

        int pid_child = fork();

        if (pid_child == -1) {
            perror("Can't fork");
            exit(3);
        }

        if (pid_child == 0) { //child

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

            switch (action->type) {
                case EXECUTABLE:
                    printf("EXECUTABLE %s\n", action->cmd);
                    exit(0);
                    //exit(1);
                    break;
                case LIBRARY:
                    printf("LIBRARY %s\n", action->cmd);
                    exit(0);
                    //exit(...);
                    break;
                case ACTION:
                    printf("ACTION %s\n", action->cmd);
                    exit(process(action->cmd);
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
        i++;
    }

    free(pstdin);
    free(pstdout);
    free(actions);

    return status;

}
