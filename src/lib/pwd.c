#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int knut_pwd(int argc, char const *argv[]) {
	char * pwd;
	pwd = (char *) get_current_dir_name();
	fprintf(stdout,"%s\n", pwd);
	free(pwd);
	return 0;
}

int main(int argc, char const *argv[])
{
	return knut_pwd(argc, argv);
}