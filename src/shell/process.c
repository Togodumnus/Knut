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
#include "utils.h"
#include "libs.h"
#include "extractionActions.h"
#include "lectureAction.h"

extern enum execution_mode EXEC_MODE;

const int PIPE_READ  = 0;
const int PIPE_WRITE = 1;

/**
 * exec
 *
 * @param   {Action *}  action      L'action a exécuter
 * @param   {Command *} cmd         La commande qui va avec
 */
void exec(Action *action, Command *cmd) {

    DEBUG("[child] EXEC_MODE %d", EXEC_MODE);

    if (cmd->type == SIMPLE) {

        if (EXEC_MODE == EXECUTABLE_MODE) {
            DEBUG("[child] EXECUTABLE %s", action->cmd);
            execvp(cmd->cmd, cmd->argv);
            exit(1); //erreur de exec si ici
        } else { //LIB_DYNAMIC_MODE ou LIB_STATIC_MODE

            DEBUG("[child] LIB %s", action->cmd);
            CommandeFonction libFunc = findCommande(cmd->cmd);

            if (libFunc == NULL) { //cmd n'existe pas dans nos libs,
                                   //on tente un execvp
                DEBUG("[child] NO LIB %s", action->cmd);
                DEBUG("[child] EXECUTABLE %s", action->cmd);
                execvp(cmd->cmd, cmd->argv);
                exit(1); //erreur de exec si ici
            } else {
                exit((*libFunc)(cmd->argc, cmd->argv));
            }
        }

    } else if (cmd->type == COMPLEX) {
        DEBUG("[child] COMPLEX CMD %s", action->cmd);
        exit(process(action->cmd, fileno(stdin), fileno(stdout)));
    } else {
        perror("Unknown action type");
        exit(1);
    }
}

//TODO write doc
int process(char *str, int fdInput, int fdOutput) {

    DEBUG("process command (in:%d, out:%d): %s", fdInput, fdOutput, str);

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

    pstdin[PIPE_READ]   = fdInput;
    pstdin[PIPE_WRITE]  = -1;
    pstdout[PIPE_READ]  = -1;
    pstdout[PIPE_WRITE] = fdOutput;

    //On exécute et chaîne chaque action

    for (int i = 0; i < actc; i++) {

        Action *action = actions[i];
        DEBUG("[parent]\taction %d (%p): %s", i, action, action->cmd);
        Command *cmd = lectureAction(action);

        // Gestion du chaînage d'entrée/sortie
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
                pstdin[PIPE_READ] = fdInput;
                pstdin[PIPE_WRITE] = -1;
            }

            //si c'est la dernière action on redirige le flux vers stdout du shell
            //si prochaine action n'est pas pipée, idem
            if (i == actc - 1 || actions[i+1]->chainingType != CHAINING_PIPE) {
                //on attache just pstdout sur stdout
                pstdout[PIPE_WRITE] = fdOutput;
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


        //Gestion des redirections d'entrées

        DEBUG(
            "[parent]\tredirect input from file = %s",
            cmd->fromFile != NULL ? "true" : "false"
        );
        if (cmd->fromFile != NULL) {
            char *mode = "r";
            FILE *file = fopen(cmd->fromFile, mode);
            if (file == NULL) {
                perror("Redirection output : Not such file");
                exit(1);
            }
            pstdin[PIPE_READ] = fileno(file);
        }

        DEBUG(
            "[parent]\tredirect output to file = %s (apppend = %s)",
            cmd->toFile != NULL ? "true" : "false",
            cmd->appendFile ? "true" : "false"
        );
        if (cmd->toFile != NULL) {
            //mode d'ouverture : append ou non
            char *mode = "w+";
            if (cmd->appendFile) {
                mode = "a+";
            }

            FILE *file = fopen(cmd->toFile, mode);
            if (file == NULL) {
                perror("Redirection output : Not such file");
                exit(1);
            }
            DEBUG("here %d", fileno(file));
            pstdout[PIPE_WRITE] = fileno(file);
            DEBUG("here");
        }


        //Création et éxécution du sous-processus

        int pid_child = fork();

        if (pid_child == -1) {
            perror("Can't fork");
            exit(3);
        }

        if (pid_child == 0) { //child

            DEBUG("[child %d] \t%d start",i, getpid());

            /*if (action->chainingType == CHAINING_PIPE) {*/
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
            /*}*/

            //Exécution de la commande
            //@see process.c#exec()
            exec(action, cmd);

        } else {

            //Fermeture des fichiers utilisés pour la redirection
            if (cmd->fromFile != NULL) {
                close(pstdin[PIPE_READ]);
            }
            if (cmd->toFile != NULL) {
                close(pstdout[PIPE_WRITE]);
            }

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
