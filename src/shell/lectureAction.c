#include <stdio.h>

#include "lectureAction.h"
#include "Action.h"
#include "Command.h"

#include "test.h" //TODO remove

Command *lectureAction(Action *action) {
    //TODO : faire un automate qui lit action->cmd
    //pour l'instant on crée une commande fake

    if (action == &actionLsPipe) {
        return &commandLsPipe;
    } else if (action == &actionEchoOr) {
        return &commandEchoOr;
    } else if (action == &actionYesAnd) {
        return &commandYesAnd;
    } else if (action == &actionCatPipe) {
        return &commandCatPipe;
    } else if (action == &actionCatFromFile) {
        return &commandCatFromFile;
    } else if (action == &actionCatToFile) {
        return &commandCatToFile;
    } else if (action == &actionEchoAndBkg) {
        return &commandEchoAndBkg;
    }

    return NULL;
}

