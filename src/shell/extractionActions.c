#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../DEBUG.h"
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

    DEBUG("createAction length %d", length);

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

    DEBUG("pushAction (*actc=%d)", *actc);

    (*actc)++;
    *actions = (Action **) realloc(*actions, (*actc) * sizeof(Action*));

    DEBUG("Push action to %d",  (*actc) - 1);
    (*actions)[(*actc) - 1] = action; //on lie action
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

    DEBUG("Début extraction action");

    //TODO : faire un automate à état fini pour lire les actions
    //séparées par |, ||, && ou ;

    enum EXTRACTION_STATES state = READING;

    int length = -1;         //taille de l'action courante
    char *startAction = str; //pointer sur le début de l'action

    ChainingType chaining = CHAINING_PIPE;
    bool background = false;

    char ch;

    do {

        length++;
        ch = *(startAction + length);

        DEBUG("current char : %c, current length : %d", ch, length);

        switch (state) {

            case READING:
                DEBUG("READING %c", ch);

                if (ch == '&') {
                    state = AND;
                } else if (ch == '|') {
                    state = PIPE;
                } else if (ch == ';') {

                    Action *currentAction = createAction(
                        startAction,
                        length - 1,
                        chaining,
                        background
                    );

                    chaining = CHAINING_COMMA;

                    pushAction(currentAction, actions, actc);

                    startAction = startAction + length + 1;
                    background = false;
                    length = -1;
                    state = READING;

                }/* else if (ch == '(') {*/

                /*}*/
                break;

            case AND:

                DEBUG("AND %c", ch);

                if (ch == '&') {

                    DEBUG("&& trouvé");

                    Action *currentAction = createAction(
                        startAction,
                        length - 2, //on ne prend pas le &&
                        chaining,
                        background
                    );

                    chaining = CHAINING_AND;

                    pushAction(currentAction, actions, actc);

                    startAction = startAction + length + 2;
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

                DEBUG("PIPE %c", ch);

                if (ch == '|') {

                    DEBUG("|| trouvé");

                    Action *currentAction = createAction(
                        startAction,
                        length - 2, //on ne prend pas le ||
                        chaining,
                        background
                    );

                    chaining = CHAINING_COMMA;

                    pushAction(currentAction, actions, actc);

                    startAction = startAction + length + 2;
                    background = false;
                    length = -1;
                    state = READING;
                } // TODO |
                break;

            case ERROR:
                DEBUG("ERROR state");
            default:
                DEBUG("DEFAULT state");
                fprintf(stderr, "Can't read command %s\n", str);
                exit(1);
                break;
        }

    } while (ch != '\n' && ch != '\0');

    Action *currentAction = createAction(
        startAction,
        length, //on ne prend pas le \n
        chaining,
        background
    );

    pushAction(currentAction, actions, actc);

    DEBUG("END of switch");

    //pour l'instant on crée une action fake

    /**actc = 2;*/

    /**actions = (Action **) malloc(*actc * sizeof(Action*));*/
    /*(*actions)[0] = &actionLsPipe;*/
    /*(*actions)[1] = &actionCatPipe;*/
    /*(*actions)[2] = &actionYesAnd;*/

    //ls / | cat && yes


    for (int i = 0; i < *actc; i ++) {
        printf("action %d = %s\n", i, (*actions)[i]->cmd);
    }

    exit(1);

}
