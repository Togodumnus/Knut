#include <stdio.h>

int main(int argc, char const *argv[])
{
	if (argc<2) { 
		printf("\n");
		return 0;
	}
	printf("%s\n",argv[1]);
	return 0;
}