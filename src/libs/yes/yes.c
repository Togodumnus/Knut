#include <stdio.h>

int yesLib(int argc, char *argv[]) {
    if (argc > 1) {
        while (42) {
            printf("%s\n", argv[1]);
        }
    } else {
        while (42) {
            printf("yes\n");
        }
    }

}
