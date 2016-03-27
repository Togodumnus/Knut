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

// ; cat < toto.txt
Action actionCatFromFile = {
    "cat",
    CHAINING_COMMA,
    false,
};

// | /bin/cat > toto.txt
Action actionCatToFile = {
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
    SIMPLE,
    "/bin/ls",
    2,
    argvLsPipe,
};

char *argvEchoOr[] = {"echo", "error", NULL};
Command commandEchoOr = {
    SIMPLE,
    "echo",
    2,
    argvEchoOr,
    NULL,
    NULL,
    false
};

char *argvYesAnd[] = {"yes", NULL};
Command commandYesAnd = {
    SIMPLE,
    "yes",
    1,
    argvYesAnd,
    NULL,
    NULL,
    false
};

char *argvCatPipe[] = {"/bin/cat", NULL};
Command commandCatPipe = {
    SIMPLE,
    "/bin/cat",
    1,
    argvCatPipe,
    NULL,
    NULL,
    false
};

char *argvCatFromFile[] = {"cat", NULL};
Command commandCatFromFile = {
    SIMPLE,
    "cat",
    1,
    argvCatFromFile,
    "toto.txt",
    NULL,
    false
};

char *argvCatToFile[] = {"/bin/cat", NULL};
Command commandCatToFile = {
    SIMPLE,
    "/bin/cat",
    1,
    argvCatPipe,
    NULL,
    "toto.txt",
    true
};

char *argvEchoAndBkg[] = {"/bin/echo", "salut", NULL};
Command commandEchoAndBkg = {
    SIMPLE,
    "/bin/echo",
    2,
    argvEchoAndBkg,
    NULL,
    NULL,
    false
};

