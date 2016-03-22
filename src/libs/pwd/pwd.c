#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int kpwd(int argc, char const *argv[]) {
	char * pwd;
	pwd = (char *) get_current_dir_name();
	printf("%s\n", pwd);
	free(pwd);
	return 0;
}