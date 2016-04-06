#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

int usage() {

    printf("\
Knut pwd \n\
\n\
usage: pwd\n\
");

    return 0;
}

int kpwd(int argc, char *argv[]) {
    int opt = 0;

    while ((opt = getopt(argc, argv, "")) != -1) {
        switch (opt) {
            default:
                return usage();
                break;
        }
    }

	char *pwd = NULL;
    size_t n = 0;
	pwd = (char *) getcwd(pwd, n);
    if (pwd == NULL) {
        perror("get_current_dir_name failure");
        exit(EXIT_FAILURE);
    }

	printf("%s\n", pwd);

	free(pwd);
	return 0;
}
