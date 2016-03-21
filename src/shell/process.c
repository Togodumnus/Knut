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

#include "../DEBUG.h"
#include "Action.h"
#include "Command.h"
#include "extractionActions.h"
#include "lectureAction.h"


const int PIPE_READ  = 0;
const int PIPE_WRITE = 1;

int process(char *str) {

    DEBUG("%d, %s", 120, "hello");

    //Extraction des actions à effectuer de l'input
    Action **actions = NULL;
    int actc = 0; //nombre d'actions
    extractionActions(str, &actions, &actc);
    DEBUG("[parent]\t%d actions to do", actc);

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
        DEBUG("[parent]\taction %d (%p): %s", i, action, action->cmd);
        Command *cmd = lectureAction(action);

        DEBUG("[parent]\tchaining type = %d", action->chainingType);

        if (action->chainingType == CHAINING_PIPE) {
            if (i > 0) {
                if (i > 1) {
                    close(pstdin[0]);
                    close(pstdin[1]);
                }
                pstdin[PIPE_READ] = pstdout[PIPE_READ];
                pstdin[PIPE_WRITE] = pstdout[PIPE_WRITE];
                close(pstdin[PIPE_WRITE]);
            } else {
                //la première action est branchée sur stdin
                pstdin[PIPE_READ] = STDIN_FILENO;
                pstdin[PIPE_WRITE] = -1;
            }

            //si c'est la dernière action on redirige le flux vers stdout du shell
            //si prochaine action n'est pas pipée, idem
            if (i == actc - 1 || actions[i+1]->chainingType != CHAINING_PIPE) {
                //on attache just pstdout sur stdout
                pstdout[PIPE_WRITE] = STDOUT_FILENO;
                pstdout[PIPE_READ] = -1;
            } else {
                //on crée un pipe pour le nouveau stdout
                if (pipe(pstdout) == -1) {
                    perror("Error pipe creation");
                    exit(2);
                }
            }
        } else if (action->chainingType == CHAINING_AND && status != 0) {
            //(error) && action, on s'arrête
            break;
        } else if (action->chainingType == CHAINING_OR && status == 0) {
            //(pas d'error) || action, on s'arrête
            break;
        }
        //else action->chainingType == COMMA, on ne fait rien de spécial

        int pid_child = fork();

        if (pid_child == -1) {
            perror("Can't fork");
            exit(3);
        }

        if (pid_child == 0) { //child

            DEBUG("[child %d] \t%d start",i, getpid());

            if (action->chainingType == CHAINING_PIPE) {
                DEBUG("[child %d] \tReplacing stdin (%d) with %d",
                        i, fileno(stdin), pstdin[PIPE_READ]);
                DEBUG("[child %d] \tReplacing stdout (%d) with %d",
                        i, fileno(stdout), pstdout[PIPE_WRITE]);

                //replace stdin
                if (pstdin[PIPE_READ] != fileno(stdin)) {
                    close(fileno(stdin));
                    dup2(pstdin[PIPE_READ], fileno(stdin));
                    close(pstdin[PIPE_READ]);
                }
                //replace stdout
                if (pstdout[PIPE_WRITE] != fileno(stdout)) {
                    close(fileno(stdout));
                    dup2(pstdout[PIPE_WRITE], fileno(stdout));
                    close(pstdout[PIPE_WRITE]);
                }
            }

            switch (cmd->type) {
                case EXECUTABLE:
                    DEBUG("[child %d] EXECUTABLE %s", i, action->cmd);
                    execvp(cmd->cmd, cmd->argv);
                    exit(1); //erreur de exec si ici
                    break;
                case LIBRARY:
                    DEBUG("[child %d] LIBRARY %s", i, action->cmd);
                    exit(0);
                    //exit(...);
                    break;
                case ACTION:
                    DEBUG("[child %d] ACTION %s", i, action->cmd);
                    exit(process(action->cmd));
                    break;
                default:
                    perror("Unknown action type");
                    exit(1);
                    break;
            }

        } else {

            //Si l'action doit s'effectuer en background
            //on n'attend pas et on met le status à 0
            if (action->background) {
                //TODO : le child va resté en zoombie ici jusqu'à la mort
                //du shell, est-ce que c'est grave ?
                status = 0;
            } else {
                //Attente de la terminaison du fils
                waitpid(pid_child, &status, 0);
                DEBUG("[parent]\tChild %s %d exited with code %d",
                        action->cmd, pid_child, status);
            }
        }

        /*free(action);*/
    }

    DEBUG("[parent] end");

    free(pstdin);
    free(pstdout);
    free(actions);

    return status;

}
