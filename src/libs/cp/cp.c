#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>

const int BUFFER_SIZE = 1024;

/**
 * appendToStr
 *
 * Ajout de ch à la fin de str
 *
 * @param  {char *}     str     La chaîne à compléter
 * @param  {char}       ch      Le caractère à ajouter
 */
void appendToStr(char *str, char ch) {

    int l = strlen(str);

    char *str = (char *) realloc(str, (l + 1) * sizeof(char));
    if (str == NULL) {
        perror("Realloc error");
        exit(EXIT_FAILURE);
    }

    str[l-1] = ch;
    str[l] = '\0';

}

/**
 * kcp_file_to_file
 *
 * Copie le contenu d'un fichier dans un autre
 *
 * @param  {char *}     path1   La source
 * @param  {char *}     path2   La destination
 */
int kcp_file_to_file(const char *path1, const char *path2) {

	FILE * f1 = fopen(path1, "r");
	FILE * f2 = fopen(path2, "w");

    if (f1 == NULL) {
        printf("Can't open %s\n", path1);
        exit(EXIT_FAILURE);
    } else if (f2 == NULL) {
        printf("Can't open %s\n", path2);
        exit(EXIT_FAILURE);
    }

    //la copie
    int n;
    char buf[BUFFER_SIZE];
    while ((n = read(fileno(f1), buf, BUFFER_SIZE)) > 0 ) {
        int nw = 0;
        do {
            nw = write(fileno(f2), buf + nw, n - nw);
            if (nw == -1) {
                printf("Write error\n");
                break;
            }
        } while (nw != n);
    }

    if (n == 1) {
        printf("Read error\n");
    }

  	fclose(f1);
   	fclose(f2);
   	return 0;
}

/**
 * kcp_file_to_dir
 *
 * Copie un fichier dans un répertoire
 *
 * @param  {char *}     file_path   La source
 * @param  {char *}     dir_path   La destination
 */
int kcp_file_to_dir(char *file_path, char *dir_path) {
	FILE * f;

    // au cas ou l'utilisateur a entré le chemin sans / à la fin
    // (/home/user au lieu de /home/user/)
	if (dir_path[strlen(dir_path)-1]!='/') {
		strcat(dir_path, "/");
	}

    char *base = basename(file_path);
    // Besoin d'une 2eme variable car on change dir_path
	char file_pathFull[strlen(dir_path) + strlen(base)];
	strcpy(file_pathFull, dir_path);
	strcat(file_pathFull, base); // file_pathFull devient le chemin du fichier

	// la c'est bon
	printf("avant %s\n", dir_path);
	if ((f=fopen(file_pathFull, "w"))==NULL) { // création du fichier
        printf("Can't open %s\n", file_pathFull);
        exit(EXIT_FAILURE);
	}
	// la c'est plus bon
	printf("après %s\n", dir_path);
	fclose(f);
	kcp_file_to_file(file_path, file_pathFull); // puis on copie le fichier dans la nouveau fichier créé
	memset (file_pathFull, 0, sizeof (file_pathFull)); // on vide le chemin du fichier
	return 0;
}

/**
 * kcp_files_to_dir
 *
 * Copie plusieurs fichiers en entrée dans un répertoire
 *
 * @param  {int *}     	  argc   Le nombre d'argument
 * @param  {char const*}  argv   Les arguments
 */
int kcp_files_to_dir(int argc, char * const argv[]) {
	struct stat st;
	int i;
	for (i = 1; i < argc-1; i++){
		// Pour savoir si ce n'est pas un dossier que l'on essaye de copier
		if (lstat(argv[i], &st)==-1) {
			printf("error lstat\n");
			exit(EXIT_FAILURE);
		}
		if (S_ISDIR(st.st_mode)) { // repertoire
			printf("kcp: omitting directory %s\n",argv[i]);
			exit(EXIT_FAILURE);
		}

		kcp_file_to_dir(argv[i],argv[argc-1]);
	}
	return 0;
}

/**
 * kcp_dir_to_dir
 *
 * Copie un répertoire dans un répertoire
 *
 * @param  {char *}  dir_path_src  	 Le chemin du répertoire à copier
 * @param  {char *}  dir_path_dest   Le chemin du répertoire de destination
 */
int kcp_dir_to_dir(char *dir_path_src, char *dir_path_dest) {
	DIR *dirp_src;
	struct dirent *dptr_src;
	struct stat st;
	char * path_to_copy;

	if ((dirp_src=opendir(dir_path_src))==NULL) { // on ouvre le dossier sourc
		printf("Can't open directory %s", dir_path_src);
		exit(EXIT_FAILURE);
	}

	if (dir_path_dest[strlen(dir_path_dest)-1]!='/') {
		strcat(dir_path_dest, "/");
	}

	if ((dir_path_src[0]=='/')||(dir_path_src[0]=='.')) { // chemin absolu
		path_to_copy = basename(dir_path_src);
	}
	else
		path_to_copy = dir_path_src;

	// Pour créé les repertoire qui seront copié
	char * make_dir = malloc(sizeof (path_to_copy) + sizeof(dir_path_dest));
	strcat(make_dir, dir_path_dest);
	strcat(make_dir, path_to_copy);
	mkdir(make_dir, 0775);


	char path[strlen(dir_path_src)]; // pour avoir le path des fichiers du dossier
	while ((dptr_src=readdir(dirp_src))) { // on lit le dossier source
		if ((strcmp(dptr_src->d_name,"..")!=0)&&(strcmp(dptr_src->d_name,".")!=0)) { // on ne tiens pas compte de . et ..

			memset(path, 0, sizeof(path));

			if (path_to_copy[strlen(dir_path_src)-1]=='/'){
				sprintf(path,"%s%s",dir_path_src,dptr_src->d_name);
			}
			else {
				sprintf(path,"%s/%s",dir_path_src,dptr_src->d_name);
			}

			if (lstat(path, &st)==-1) {
				printf("error lstat\n");
				exit(EXIT_FAILURE);
			}
			if (S_ISDIR(st.st_mode)) { // repertoire
				kcp_dir_to_dir(path, make_dir);
			}
			else { // fichier
				kcp_file_to_dir(path, make_dir);
			}
		}
	}
	free(make_dir);
	closedir(dirp_src);
	return 0;
}

/**
 * kcp_dirs_to_dir
 *
 * Copie plusieurs répertoires en entrée dans un repertoire
 *
 * @param  {int *}     	  argc   Le nombre d'argument
 * @param  {char const*}  argv   Les arguments
 */
int kcp_dirs_to_dir(int argc, char * const argv[]) {
	int i;
	for (i = 2; i < argc-1; i++){
		kcp_dir_to_dir(argv[i],argv[argc-1]);
	}
	return 0;
}

/**
 * kcp
 *
 * Fonction de base
 * Permet d'appeler les bonnes fonctions et de traiter les options
 *
 * @param  {int *}     	  argc   Le nombre d'argument
 * @param  {char const*}  argv   Les arguments
 */
int kcp(int argc, char * const argv[]) {
	struct stat st;
	char c;
	while((c = getopt(argc, argv, "r")) != -1) {
		switch(c) {
			case 'r':
				return kcp_dirs_to_dir(argc, argv);
				break;
			case '?': //option no reconnue
				exit(EXIT_FAILURE);
		}
	}
	if (lstat(argv[argc-1], &st)==-1) {
		printf("error lstat\n");
		exit(EXIT_FAILURE);
	}
	if (S_ISDIR(st.st_mode)) { // repertoire
		return kcp_files_to_dir(argc, argv);
	}
	else  {
		if (argc>3) {
			printf("cp: target %s is not a directory\n", argv[argc-1]);
			exit(EXIT_FAILURE);
		}
		return kcp_file_to_file(argv[1], argv[2]);
	}
}

int main(int argc, char * const argv[]) {
	return kcp(argc, argv);
}
