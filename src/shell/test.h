
#include "stdbool.h"

#include "Action.h"
#include "Command.h"

// | /bin/ls /
Action actionLsPipe;

// || echo error
Action actionEchoOr;

// && yes
Action actionYesAnd;

// | /bin/cat
Action actionCatPipe;

// ; cat < toto.txt
Action actionCatFromFile;

// | /bin/cat > toto.txt
Action actionCatToFile;

// && echo salut &
Action actionEchoAndBkg;

Command commandLsPipe;

Command commandEchoOr;

Command commandYesAnd;

Command commandCatPipe;

Command commandCatFromFile;

Command commandCatToFile;

Command commandEchoAndBkg;

