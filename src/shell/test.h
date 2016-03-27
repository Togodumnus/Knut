
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

// | /bin/cat > toto.txt
Action actionCatToFile;

// && echo salut &
Action actionEchoAndBkg;

Command commandLsPipe;

Command commandEchoOr;

Command commandYesAnd;

Command commandCatPipe;

Command commandCatToFile;

Command commandEchoAndBkg;

