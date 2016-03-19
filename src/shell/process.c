#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

#include "process.h"
#include "Action.h"

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
 * @param   {char *}    str     Un pointeur sur la chaîne d'entrée
 * @return  {Action **} actions Un pointeur un futur tableau de pointeurs sur
 *                              les actions. Le dernier pointeur de actions doit
 *                              être NULL.
 */
Action **extractionActions(char *str) {

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

    Action **actions = (Action **) malloc(3 * sizeof(Action*));
    actions[0] = action;
    actions[1] = action2;
    actions[2] = NULL;

    return actions;
}

void process(char *str) {

    //Extraction des actions à effectuer de l'input
    Action **actions = extractionActions(str);

    int i = 0;
    do {
        Action *action = actions[i];

        printf("action : %s\n", action->cmd);

        free(action);
        i++;
    } while (actions[i] != NULL);

    free(actions);

}
