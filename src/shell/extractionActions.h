#pragma once

#include "Action.h"

void extractionActions(char *str, Action ***actions, int *actc);

enum EXTRACTION_STATES {
    START,
    READING,
    AND,
    PIPE,
    BRACKET,
    ERROR
};
