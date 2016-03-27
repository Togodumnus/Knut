#include <stdio.h>
#include <stdlib.h>

#include "extractionActions.h"
#include "Action.h"
#include "test.h" //TODO remove

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

    *actc = 2;

    *actions = (Action **) malloc(*actc * sizeof(Action*));
    (*actions)[0] = &actionLsPipe;
    (*actions)[1] = &actionCatToFile;
    /*(*actions)[2] = &actionYesAnd;*/

    //ls / | cat && yes

}
