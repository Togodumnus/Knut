#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int kpwd(int argc, char const *argv[]) {
	char *pwd;
	pwd = (char *) get_current_dir_name();
    if (pwd == NULL) {
        perror("get_current_dir_name failure");
        exit(EXIT_FAILURE);
    }

	printf("%s\n", pwd);

	free(pwd);
	return 0;
}
