#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int kecho(int argc, char * const argv[]) {
	int opt = 0;
	int opt_n = 0; // pour savoir si on a mis l'option -n
	if (argc<2) { 
		printf("\n");
		return 0;
	}
	char c;
	while((c = getopt(argc, argv, "ne")) != -1) {
		opt++;
		switch(c) {
			case 'e':
				printf("e\n");
				break;
			case 'n': 
				opt_n = 1;
				break;
			case '?': // option pas reconnu
				exit(-1);
		}
	}
	int i;
	for (i=opt+1;i<argc;i++) {
		printf("%s ", argv[i]);
	}
	if (!opt_n) printf("\n");
	return 0;
}