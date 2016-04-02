#pragma once

#include "Action.h"
#include "Command.h"

Command *lectureAction(Action *action);

enum LECTURE_STATES {
    START,
    READING_COMMAND,
    READING_COMPLEX,
    CHEVRON,
    READING_FILE
};
