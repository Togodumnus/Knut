#pragma once

#include "Action.h"

void extractionActions(char *str, Action ***actions, int *actc);

enum EXTRACTION_STATES {
    READING,
    AND,
    PIPE,
    BRACE,
    ERROR
};
