#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "extractionActions.h"
#include "Action.h"
#include "test.h" //TODO remove


/**
 * createAction
 *
 * @see Action.h#ChainingType
 *
 * @param {char *}  start       Le pointeur sur le premier charactère de l'action
 * @param {int}     length      Le nombre de caractère à prendre
 * @param {int}     type        Une valeur de enum ChainingType
 * @param {int}     background  True si l'action doit être lancée en background
 *                              (présence d'un & à la fin)
 *
 * @return {Action *}       Un pointeur vers une action qu'il faudra free()
 *                          par la suite
 */
Action* createAction(char *start, int length, int type, bool background) {

    Action *action = (Action *) malloc(sizeof(Action));
    //on copie la commande
    action->cmd = (char *) malloc((length + 1) * sizeof(char));
    strncpy(action->cmd, start, length);
    action->cmd[length] = '\0';

    action->chainingType = type;
    action->background = background;

    return action;
}

/**
 * pushAction
 *
 * Ajout de l'action dans la liste d'action
 *
 * @param  {Action *}   action  L'action à ajouter
 * @param  {Action ***} actions La liste d'actions
 * @param  {int *}      actc    Le nombre d'action sans action
 */
void pushAction(Action *action, Action ***actions, int *actc) {
    *actions = (Action **) realloc(actions, (*actc) * sizeof(Action*));

    *actions[*actc] = action; //on lie action
    (*actc)++;
}

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

    enum EXTRACTION_STATES state = READING;

    int length = -1;         //taille de l'action courante
    char *startAction = str; //pointer sur le début de l'action

    ChainingType chaining = CHAINING_PIPE;
    bool background = false;

    while (*startAction != '\0') {

        char ch = *(startAction + length);
        length++;

        switch (state) {

            case READING:

                if (ch == '&') {
                    state = AND;
                } else if (ch == '|') {
                    state = PIPE;
                } else if (ch == ';') {

                    Action *currentAction = createAction(
                        startAction,
                        length - 1, //on ne prend pas le ;
                        chaining,
                        background
                    );

                    chaining = CHAINING_COMMA;

                    pushAction(currentAction, actions, actc);

                    startAction = startAction + length + 1;
                    background = false;
                    length = -1;
                    state = READING;

                }
                break;

            case AND:
                if (ch == '&') {

                    Action *currentAction = createAction(
                        startAction,
                        length - 2, //on ne prend pas le &&
                        chaining,
                        background
                    );

                    chaining = CHAINING_AND;

                    pushAction(currentAction, actions, actc);

                    startAction = startAction + length + 1;
                    background = false;
                    length = -1;
                    state = READING;

                } else if (ch == ' ') {
                    background = true;
                    state = READING;
                } else {
                    state = ERROR;
                }
                break;

            case PIPE:
                if (ch == '|') {

                    Action *currentAction = createAction(
                        startAction,
                        length - 2, //on ne prend pas le ||
                        chaining,
                        background
                    );

                    chaining = CHAINING_COMMA;

                    pushAction(currentAction, actions, actc);

                    startAction = startAction + length + 1;
                    background = false;
                    length = -1;
                    state = READING;
                }

            case ERROR:
            default:
                fprintf(stderr, "Can't read command %s\n", str);
                break;
        }

    }



    //pour l'instant on crée une action fake

    /**actc = 2;*/

    /**actions = (Action **) malloc(*actc * sizeof(Action*));*/
    /*(*actions)[0] = &actionLsPipe;*/
    /*(*actions)[1] = &actionCatPipe;*/
    /*(*actions)[2] = &actionYesAnd;*/

    //ls / | cat && yes

    exit(1);

}
