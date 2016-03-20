#include "stdio.h"
#include "stdbool.h"

#include "Action.h"
#include "Command.h"

// | /bin/ls /
Action actionLsPipe = {
    "/bin/ls /",
    CHAINING_PIPE,
    false
};

// || echo error
Action actionEchoOr = {
    "echo error",
    CHAINING_OR,
    false,
};

// && yes
Action actionYesAnd = {
    "yes",
    CHAINING_AND,
    false,
};

// | /bin/cat
Action actionCatPipe = {
    "/bin/cat",
    CHAINING_PIPE,
    false,
};

// && echo salut &
Action actionEchoAndBkg = {
    "echo salut",
    CHAINING_AND,
    true,
};

char *argvLsPipe[] = {"/bin/ls", "/", NULL};
Command commandLsPipe = {
    EXECUTABLE,
    "/bin/ls",
    2,
    argvLsPipe,
};

char *argvEchoOr[] = {"echo", "error", NULL};
Command commandEchoOr = {
    LIBRARY,
    "echo",
    2,
    argvEchoOr,
};

char *argvYesAnd[] = {"yes", NULL};
Command commandYesAnd = {
    LIBRARY,
    "yes",
    1,
    argvYesAnd,
};

char *argvCatPipe[] = {"/bin/cat", NULL};
Command commandCatPipe = {
EXECUTABLE,
"/bin/cat",
1,
argvCatPipe,
};

char *argvEchoAndBkg[] = {"/bin/echo", "salut", NULL};
Command commandEchoAndBkg = {
    EXECUTABLE,
    "/bin/echo",
    2,
    argvEchoAndBkg,
};

