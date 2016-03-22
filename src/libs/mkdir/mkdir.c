#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>


/*
* Convertir octal en décimal
*/
int octal_decimal(int n)
{
    int decimal=0, i=0, rem;
    while (n!=0)
    {
        rem = n%10;
        n/=10;
        decimal += rem*pow(8,i);
        ++i;
    }
    return decimal;
}

/**
* Mode verbeux
* Affiche un message avec le nom du repertoire créé
*/
int kmkdir_v(char * dirname) {
	printf("mkdir: created directory %s\n", dirname);
	return 0;
}

/**
* Ajout des permissions
* Exemple : mkdir -m 777 dir
* Créé un répértoire dir avec toutes les permissions pour tout le monde
*/
int kmkdir_m(int argc, char * const argv[]) {
	if (argc<4) {
		printf("kmkdir : missing operand\n");
		return -1;
	}
	return octal_decimal(atoi(argv[argc-2]));	
}

/*
* Pour traiter les options
*/
int kmkdir(int argc, char * const argv[]) {
	if (argc<2) {
		printf("kmkdir : missing operand\n");
		return -1;
	}
	int c;
	int permissions = 0775; // permissions de base
	while((c = getopt(argc, argv, "mv")) != -1) {
		switch(c) {
			case 'm':
				permissions = kmkdir_m(argc, argv);
				break;
			case 'v': 
				kmkdir_v(argv[argc-1]);
				break;
			case '?': // option pas reconnu
				exit(-1);	
		}
	}
	mkdir(argv[argc-1], permissions);
	return 0;
}