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
    if (*action == NULL) {
        perror("Malloc error");
        exit(EXIT_FAILURE);
    }
    action->length = length;
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

    if (*actions == NULL) {
        perror("Realloc error");
        exit(EXIT_FAILURE);
    }

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

    /**
     * state
     *
     * État de l'automate
     * @see extractionActions.h
     */
    enum EXTRACTION_STATES state = READING;

    /**
     * length
     *
     * Taille de l'action courante
     */
    int length = -1;

    /**
     * startAction
     *
     * Pointeur sur le premier caractère de l'action courante
     */
    char *startAction = str;

    /**
     * chaining
     *
     * Type de chaînage de l'action courante
     */
    ChainingType chaining = CHAINING_PIPE;

    /**
     * background
     *
     * true si l'action courante est suivie de & et doit être executée
     */
    bool background = false;

    /**
     * bracket_depth
     *
     * Sert à se situer en profondeur dans les parenthèse
     * On accèpte en effet les action du typte (ls && (echo salut; echo test))
     *                                         |------------ action ----------|
     */
    int bracket_depth = 0;

    /**
     * ch
     *
     * Caractère courant
     */
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
                    background = true; //c'est peut-être un & seul
                } else if (ch == '|') {
                    state = PIPE;
                } else if (ch == ';') {

                    //on enregistre l'action courante et on RAZ

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

                } else if (ch == '(') {
                    state = BRACKET;
                }
                break;

            case AND:

                DEBUG("AND %c", ch);

                if (ch == '&') {

                    background = false; //en fait ce n'est pas un "&" seul
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

                    chaining = CHAINING_OR;

                    pushAction(currentAction, actions, actc);

                    startAction = startAction + length + 2;
                    background = false;
                    length = -1;
                    state = READING;
                } else if (ch == ' ') {

                    DEBUG("| trouvé");

                    Action *currentAction = createAction(
                        startAction,
                        length - 2, //on ne prend pas le ||
                        chaining,
                        background
                    );

                    chaining = CHAINING_PIPE;

                    pushAction(currentAction, actions, actc);

                    startAction = startAction + length + 1;
                    background = false;
                    length = -1;
                    state = READING;

                } else {
                    state = ERROR;
                }
                break;

            case BRACKET:
                if (ch == '(') {
                    DEBUG("(++");
                    bracket_depth++;
                } else if (ch == ')') {
                    if (bracket_depth == 0) {
                        DEBUG(")-->READING");
                        state = READING;
                    } else {
                        DEBUG(")--");
                        bracket_depth--;
                    }
                }

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


    //ajout de la dernière action
    Action *currentAction = createAction(
        startAction,
        length, //on ne prend pas le \n
        chaining,
        background
    );

    pushAction(currentAction, actions, actc);

    DEBUG("END extraction action");

    //debug
    for (int i = 0; i < *actc; i ++) {
        DEBUG(
            "action %d = %s (chaining %d) (background %d)",
            i,
            (*actions)[i]->cmd,
            (*actions)[i]->chainingType,
            (*actions)[i]->background
        );
    }

}
