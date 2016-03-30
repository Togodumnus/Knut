#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int kcp_file_to_file(const char *path1, const char *path2) {
	FILE * f1 = fopen(path1, "r");
	FILE * f2 = fopen(path2, "w");

	char line[64];

	while(fgets(line, 64,f1)!=NULL) {
		fprintf(f2, line);
	}

  	fclose(f1);
   	fclose(f2);
   	return 0;
}

int kcp_file_to_dir(const char *file_name, char *dir_path) {
	if (dir_path[strlen(dir_path)-1]!='/') {
		strcat(dir_path, "/"); 
	} // au cas ou l'utilisateur à entré le chemin sans / à la fin (/home/user au lieu de /home/user/)
	char dir_pathFull[strlen(dir_path)]; // Besoin d'une 2eme variable car sinon dir_path change 

	strcpy(dir_pathFull, dir_path); 
	strcat(dir_pathFull, file_name); // dir_pathFull devient le chemin du fichier
	
	fclose(fopen(dir_pathFull, "w")); // on créé le fichier puis on ferme le FILE
	kcp_file_to_file(file_name, dir_pathFull); // puis on copie le fichier dans la nouveau fichier créé
	memset (dir_pathFull, 0, sizeof (dir_pathFull)); // on vide le chemin du fichier 
	return 0;
}

int kcp_files_to_dir(int argc, char * const argv[]) {
	int i;
	for (i = 1; i < argc-1; i++){
		kcp_file_to_dir(argv[i],argv[argc-1]);
	}
	return 0;
}

int kcp(int argc, char * const argv[]) {
	char c;
	while((c = getopt(argc, argv, "r")) != -1) {
		switch(c) {
			case 'r':
				printf("r\n");
				break;
			case '?': // option pas reconnu
				exit(-1);
		}
	}
	// TODO : vérifier que c'est pas des dossiers et que le dernier est un dossier
	return kcp_files_to_dir(argc, argv);
}

int main(int argc, char * const argv[]) {
	return kcp(argc, argv);
}