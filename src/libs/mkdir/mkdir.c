#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <libgen.h>
#include <string.h>

/**
 * octal_decimal
 *
 * Convertie un nombre decimal en base octal 
 *
 * @param  {int}  n    Le nombre à convertir
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
 * kmkdir_v
 *
 * Option 'v' de mkdir : affiche un message  
 *
 * @param  {char *}  dirname    Le nom du répertoire créé
 */
int kmkdir_v(char * dirname) {
	printf("mkdir: created directory %s\n", dirname);
	return 0;
}

/**
 * kmkdir_m
 *
 * Option 'm' de mkdir : droit préciser lors de la création du répertoire 
 *
 * @param  {int}  	   argc    Le nombre d'arguments d'entrée
 * @param  {char[] *}  argv    Les arguments d'entrée
 */
int kmkdir_m(int argc, char * argv[]) {
	if (argc<4) {
		printf("kmkdir : missing operand\n");
		exit(EXIT_FAILURE);
	}
	return octal_decimal(atoi(argv[argc-2]));	
}

/**
 * kmkdir_p
 *
 * Option 'p' de mkdir : créer les répertoires parents nécessaire
 *
 * @param  {char *}   path    		 Le chemin du répertoire à créé
 * @param  {int}  	  permissions    Les permissions du répertoire créé
 */
int kmkdir_p(char * path, int permissions) {
	if (!strcmp(dirname(path),".")) { // si path est "seul" (usr/) (création du premier dossier)
		if (mkdir(path, permissions) == -1) {
			perror("kmkdir p");
			exit(EXIT_FAILURE);
		}
		return 0;
	}
	else { // sinon on rappel kmkdir_p jusqu'à créé le premier dossier
		kmkdir_p(path, permissions);
	}
	// pour les autres dossiers
	if (mkdir(path, permissions) == -1) {
			perror("kmkdir p2");
			exit(EXIT_FAILURE);
	}
	return 0;	
}

/**
 * kmkdir
 *
 * Fonction principale pour traiter les options
 *
 * @param  {int}  	   argc    Le nombre d'arguments d'entrée
 * @param  {char[] *}  argv    Les arguments d'entrée
 */
int kmkdir(int argc, char * argv[]) {
	if (argc<2) {
		printf("kmkdir : missing operand\n");
		exit(EXIT_FAILURE);
	}
	int c;
	int vFlag = 0;
	int permissions = 0775; // permissions de base
	while((c = getopt(argc, argv, "pmv")) != -1) {
		switch(c) {
			case 'm':
				permissions = kmkdir_m(argc, argv);
				break;
			case 'v': 
				vFlag = 1;
				break;
			case 'p':
				kmkdir_p(argv[argc-1], permissions);
			case '?': // option pas reconnu
				exit(EXIT_FAILURE);	
		}
	}
	if (mkdir(argv[argc-1], permissions) == -1) {
		perror("kmkdir");
		exit(EXIT_FAILURE);
	}
	if (vFlag) 
		kmkdir_v(argv[argc-1]);
	return 0;
}

int main(int argc, char * argv[]) {
	return kmkdir(argc, argv);
}