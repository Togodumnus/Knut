#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


int kcp_file_to_file(const char *path1, const char *path2) {
	FILE * f1 = fopen(path1, "r");
	FILE * f2 = fopen(path2, "w");

	char line[64];

	while(fgets(line, 64,f1)!=NULL) {
		fprintf(f2,"%s",line);
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

int kcp_dir_to_dir(char *dir_path_src, char *dir_path_dest) {
	DIR *dirp_src;
	struct dirent *dptr_src;
	struct stat st;

	if ((dirp_src=opendir(dir_path_src))==NULL) { // on ouvre le dossier sourc
		printf("Erreur lors de l'ouverture du dossier");
		return(-1);
	}
	// Pour créé les repertoire qui seront copié 
	char make_dir[strlen(dir_path_src)+strlen(dir_path_dest)];
	memset (make_dir, 0, sizeof (make_dir));
	strcat(make_dir, dir_path_dest);
	strcat(make_dir, dir_path_src);
	mkdir(make_dir, 0775);

	while ((dptr_src=readdir(dirp_src))) { // on lit le dossier source
		char path[strlen(dir_path_src)]; // pour avoir le path des fichiers du dossier

		if (dir_path_src[strlen(dir_path_src)-1]=='/'){ 
			sprintf(path,"%s%s",dir_path_src,dptr_src->d_name);
		}
		else {		
			sprintf(path,"%s/%s",dir_path_src,dptr_src->d_name);
		}

		if (lstat(path, &st)==-1) {
			printf("erreur stat");
			exit(-1);
		}

		if (S_ISREG(st.st_mode)) { // fichier "normal"
			kcp_file_to_dir(path, dir_path_dest);
		}
		else if (S_ISDIR(st.st_mode)) { // repertoir
			//kcp_dir_to_dir(path, dir_path_dest);
			// boucle infini 
		}
		else { // autre (liens, pipe, device...)
			// à copier aussi ?
		}
	}
	

	/*
	* Ouvrir le dossier src
	* Tester si la dest existe : sinon le créer (pour la récusrivité)
	* parcours les fichiers
		* Si fichier : kcp_file_to_dir(dir_path_src+nameFile, dir_path_dest)
		* Si dossier : appeler méthode recursif : kcp_dir_to_dir(dir_path_src, dir_path_dest)
	* 
	*/
	return 0;
}

int kcp_dirs_to_dir(int argc, char * const argv[]) {
	return kcp_dir_to_dir(argv[1], argv[2]);
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
	// TODO : vérifier que c'est sous la forme "kcp f1 f2 f3 dir1"
	// cf myls avec stat pour voir si c un dir
	return kcp_dirs_to_dir(argc, argv);
}

int main(int argc, char * const argv[]) {
	return kcp(argc, argv);
}