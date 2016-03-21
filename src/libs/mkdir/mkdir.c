#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

/**
* Fonction principale
*/
int kmkdir(int argc, char const *argv[]) {
	if (argc<2) {
		perror("Pas assez d'arguments\n");
		return -1;
	}
	mkdir(argv[1], 775);
	return 0;
}

/**
* Mode verbeux
* Affiche un message avec le nom du repertoire créé
*/
int kmkdir_v(char * dirname) {
	printf("mkdir: created directory %s\n", dirname);
}

/**
* Fonction appelé par la librairie
*/
int kmkdir_lib(int argc, char const *argv[]) {
	int ret = kmkdir(argc, argv);
	while((c = getopt(argc, argv, "mpvZ")) != -1) {
		switch(c) {
			case 'm':
				printf("m\n");
				break;
			case 'p':
				printf("p\n");
				break;
			case 'v': 
				printf("i\n");
				break;	
			case 'Z': 
				printf("i\n");
				break;	
		}
	}
	return 0;
}


int main(int argc, char const *argv[]) {
	kmkdir_lib(argc, argv);
}