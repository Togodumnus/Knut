#ifndef __APPLE__
    #define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "process.h"

#include "../DEBUG.h"
#include "Action.h"
#include "Command.h"
#include "utils.h"
#include "libs.h"
#include "server.h"
#include "extractionActions.h"
#include "lectureAction.h"
#include "shellCommands.h"

extern enum execution_mode EXEC_MODE;

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
            DEBUG("[child] EXECUTABLE %s (%d arg)", cmd->cmd, cmd->argc);
            execvp(cmd->cmd, cmd->argv);
            DEBUG("execvp error %d", errno);
            if (errno == ENOENT) {
                printf("%s : no such file \n", cmd->cmd);
            } else if (errno == EACCES) {
                printf("Can't access %s\n", cmd->cmd);
            } else {
                printf("Unknow error : can't exec %s\n", cmd->cmd);
            }
            exit(EXIT_FAILURE); //erreur de exec si ici
        } else { //LIB_DYNAMIC_MODE ou LIB_STATIC_MODE

            DEBUG("[child] LIB %s", cmd->cmd);
            CommandeFonction libFunc = findCommande(cmd->cmd);

            if (libFunc == NULL) { //cmd n'existe pas dans nos libs,
                                   //on tente un execvp
                DEBUG("[child] NO LIB %s", cmd->cmd);
                DEBUG("[child] EXECUTABLE %s", cmd->cmd);
                execvp(cmd->cmd, cmd->argv);
                DEBUG("execvp error");
                exit(EXIT_FAILURE); //erreur de exec si ici
            } else {

                char **p;
                for (p = cmd->argv; *p; p++) {
                    DEBUG("arg %p : %s", p, *p);
                }

                int status = (*libFunc)(cmd->argc, cmd->argv);
                exit(status);
            }
        }

    } else if (cmd->type == COMPLEX) {
        DEBUG("[child] COMPLEX CMD %s", action->cmd);
        exit(process(cmd->cmd, fileno(stdin), fileno(stdout)));
    } else {
        perror("Unknown action type");
        exit(EXIT_FAILURE);
    }
}

/**
 * process
 *
 * Éxécution de la commande
 *
 * @param  {char *} str         La commande à exécuter
 * @param  {int}    fdInput     File descriptot
 * @param  {int}    fdOutput    File descriptot
 *
 * @return {int}            Status de retour
 */
int process(char *str, int fdInput, int fdOutput) {

    DEBUG("process command (in:%d, out:%d): %s", fdInput, fdOutput, str);

    //Extraction des actions à effectuer de l'input
    Action **actions = NULL;
    int actc = 0; //nombre d'actions
    extractionActions(str, &actions, &actc);
    DEBUG("[parent]\t%d actions to do", actc);

    int status = 0; //status code of execution

    //let's create pipes to chain stdin and stdout
    int pstdin[2],
        pstdout[2];

    pstdin[PIPE_READ]   = fdInput;
    pstdin[PIPE_WRITE]  = -1;
    pstdout[PIPE_READ]  = -1;
    pstdout[PIPE_WRITE] = fdOutput;

    //On exécute et chaîne chaque action

    for (int i = 0; i < actc; i++) {

        FILE *file = NULL; //pour redirection

        Action *action = actions[i];
        Command *cmd = lectureAction(action);
        DEBUG("[parent]\taction %d (%p): %s", i, action, action->cmd);
        DEBUG("[parent] cmd : %s", cmd->cmd);

        //On autorise l'utisateur à utiliser le caractère \n pour nettoyer
        //l'écran
        if (actc == 1 && cmd->cmd == NULL) {
            return 0;
        }

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

        //cf. man bash, "Here Documents"
        DEBUG(
            "[parent]\tredirect input from file = %s",
            cmd->fromFile != NULL ? "true" : "false"
        );

        //here documement
        if (cmd->fromFile != NULL && cmd->appendFile) {
            //cmd->fromFile n'est pas un fichier ici mais le code de stop
            int length = strlen(cmd->fromFile);
            DEBUG(
                "[parent]\there document, (stop=%s(%d))",
                cmd->fromFile, length
            );

            //on modifie le stop pour le comparer avec une ligne de l'input
            char stop[length + 2];
            strcpy(stop, cmd->fromFile);
            stop[length] = '\n';
            stop[length + 1] = '\0';
            DEBUG("stop word = %s", stop);

            //on crée un pipe pour stocker l'entrée
            int tmpPipe[2];
            if (pipe(tmpPipe) == -1) {
                perror("Error pipe creation");
                exit(EXIT_FAILURE);
            }

            size_t n;
            char *line = NULL;

            //on lit l'input tant qu'on ne reçoit pas le stop
            do {
                dprintf(fdOutput, YELLOW "➜ " END);

                if (line != NULL) { //pas le 1er getline
                    write(tmpPipe[PIPE_WRITE], line, n);
                }

                n = 0;
                if (isSocket(fdInput)) { //c'est un socket
                    n = getLineSocket(&line, &n, fdInput);
                } else { //c'est stdin
                    n = getline(&line, &n, stdin);
                }
                DEBUG("get line = %s", line);

            } while (strlen(line) > 0 && strcmp(line, stop) != 0);

            close(tmpPipe[PIPE_WRITE]);

            pstdin[PIPE_READ] = tmpPipe[PIPE_READ];

        } else if (cmd->fromFile != NULL) {

            char *mode = "r";
            FILE *file = fopen(cmd->fromFile, mode);
            if (file == NULL) {
                perror("Redirection output : Not such file");
                exit(EXIT_FAILURE);
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
                exit(EXIT_FAILURE);
            }
            DEBUG("here %d", fileno(file));
            pstdout[PIPE_WRITE] = fileno(file);
            DEBUG("here");
        }

        //Création et éxécution du sous-processus

        //une commande built-in
        int e;
        if ((e = shellCommand(cmd)) != -1) {
            status = e;
            break;
        }

        //sinon, on lance un process fils
        int pid_child = fork();

        if (pid_child == -1) {
            perror("Can't fork");
            exit(3);
        }

        if (pid_child == 0) { //child

            DEBUG("[child %d] \t%d start",i, getpid());

            DEBUG("[child %d] \tReplacing stdin (%d) with %d",
                    i, fileno(stdin), pstdin[PIPE_READ]);
            DEBUG("[child %d] \tReplacing stdout (%d) with %d",
                    i, fileno(stdout), pstdout[PIPE_WRITE]);

            //replace stdin
            if (pstdin[PIPE_READ] != fileno(stdin)) {
                close(fileno(stdin));
                dup2(pstdin[PIPE_READ], fileno(stdin));
            }
            //replace stdout
            if (pstdout[PIPE_WRITE] != fileno(stdout)) {
                close(fileno(stdout));
                dup2(pstdout[PIPE_WRITE], fileno(stdout));
            }

            //en mode socket on veut rediriger stderr sur le client
            if (isSocket(fdInput)) {
                dup2(pstdout[PIPE_WRITE], fileno(stderr));
            }

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
                //TODO : le child peut rester en zoombie
                dprintf(fdOutput, "Process %d in background\n", pid_child);
                status = 0;
            } else {
                //Attente de la terminaison du fils
                waitpid(pid_child, &status, 0);
                DEBUG("[parent]\tChild %s %d exited with code %d",
                        action->cmd, pid_child, status);
            }
        }

        if (file != NULL) {
            fclose(file);
        }

        freeCommand(cmd);
        freeAction(action);
    }

    DEBUG("[parent] end");

    free(actions);

    return status;

}
