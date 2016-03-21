#include <stdio.h>
#include <stdlib.h>

#include "../DEBUG.h"
#include "libs.h"
#include "front.h"
#include "process.h"

int main(int argc, char* argv[]) {

    printf("KnutShell");

    #ifndef LIB_STATIC
        //si pas de LIB_STATIC, on charge les librairies dynamiquement
        loadLibs();
        printf(" (librairies dynamiques)\n");
    #else
        printf(" (librairies statiques)\n");
    #endif

    char *line = NULL;
    while (42) {
        printPrompt();
        if (readLine(&line) == -1) { //End of file
            printf("\nBye !\n");
            free(line);
            exit(1);
        } else {
            DEBUG("User: %s", line);
            process(line);
        }
    }

}
