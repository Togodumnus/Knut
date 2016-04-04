#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

/**
* mountName
* 
* Retourne le nom du mois 
* 
 * @param  {int}  nb   Le numéro du mois 
 */
char * mouthName(int nb) {
	switch(nb) {
		case 0:
			return "janv.";
		case 1:
			return "févr.";
		case 2:
			return "mars";
		case 3:
			return "avril";
		case 4:
			return "mai";
		case 5:
			return "juin";
		case 6:
			return "juill.";
		case 7:
			return "août";
		case 8:
			return "sept.";
		case 9:
			return "oct.";
		case 10:
			return "nov.";
		case 11:
			return "déc.";
	}
	return "Error";
}

int kls_no_opt(int argc, char * argv[]) {
	return 0;
}

int kls_l(int argc, char * argv[]) {
	return 0;
}

int kls_a(int argc, char * argv[]) {
	return 0;
}

int kls_al(int argc, char * argv[]) {
	DIR *dirp;
	struct dirent *dptr;

	struct stat statls;
	struct passwd *user;
	struct group *grd;

	struct tm *tmInfo;

	char *path;
	
	if (kargv[1]==NULL) {
		path = "."; // repetoire courant
	}
	else {
		path = kargv[1];
	}

	if ((dirp=opendir(path))==NULL) {
		printf("Error\n");
		return -1;
	}

	if (kargc<3) {
		while ((dptr=readdir(dirp))) {
			stat(dptr->d_name, &statls);
			
			// ne marche pas
		/*	if ((statls.st_mode & S_IFSOCK)==S_IFSOCK) printf("s");	// socket
			else if ((statls.st_mode & S_IFLNK)==S_IFLNK) printf("l");	// symbolic link
			else if ((statls.st_mode & S_IFREG)==S_IFREG) printf("-");	// regular file
			else if ((statls.st_mode & S_IFBLK)==S_IFBLK) printf("b");	// block device
			else if ((statls.st_mode & S_IFDIR)==S_IFDIR) printf("d");	// directory
			else if ((statls.st_mode & S_IFCHR)==S_IFCHR) printf("c");	// character device
			else if ((statls.st_mode & S_IFIFO)==S_IFIFO) printf("p");	// FIFO*/

			printf("%c",(statls.st_mode & S_IRUSR)==S_IRUSR ? 'r' : '-');	// owner R
			printf("%c",(statls.st_mode & S_IWUSR)==S_IWUSR ? 'w' : '-');	// owner W
			printf("%c",(statls.st_mode & S_IXUSR)==S_IXUSR ? 'x' : '-');	// owner X
			printf("%c",(statls.st_mode & S_IRGRP)==S_IRGRP ? 'r' : '-');	// group R
			printf("%c",(statls.st_mode & S_IWGRP)==S_IWGRP ? 'w' : '-');	// group W
			printf("%c",(statls.st_mode & S_IXGRP)==S_IXGRP ? 'x' : '-');	// group X
			printf("%c",(statls.st_mode & S_IROTH)==S_IROTH ? 'r' : '-');	// other R
			printf("%c",(statls.st_mode & S_IWOTH)==S_IWOTH ? 'w' : '-');	// other W
			printf("%c",(statls.st_mode & S_IXOTH)==S_IXOTH ? 'x' : '-');	// other X

			printf(" %d", statls.st_nlink); // number of hard links

			user = getpwuid(statls.st_uid); // user name
			printf(" %s", user->pw_name);

			grd = getgrgid(statls.st_gid); // group name
			printf(" %s", grd->gr_name);

			printf(" %d", statls.st_size); // taille

			tmInfo =  localtime(&statls.st_mtime); 
			printf(" %s %d %d:%d",mouthName(tmInfo->tm_mon), tmInfo->tm_mday, tmInfo->tm_hour, tmInfo->tm_min);
			printf(" %s\n",dptr->d_name);
		}
	}
	else {
		printf("Error too many arguments\n");
		exit(EXIT_FAILURE);
	}
	closedir(dirp);
	return 0;
}

int kls(int argc, char *argv[]) {
	char c;

	int aFlag, lFlag = 0

	while((c = getopt(argc, argv, "al")) != -1) {
		switch(c) {
			case 'a':
				aFlag = 1;
				break;
			case 'l':
				lFlag = 1;
				break;
			case '?':
				printf("kls: option requires an argument -- '%c'\n",c);
		}
		if ((aFlag)&&(lFlag)) {
			return kls_al(argc, argv);
		}
		else if (aFlag) {
			return kls_a(argc, argv);
		}
		else if (lFlag) {
			return kls_l(argc, argv);
		}
		else {
			return kls_no_opt(argc, argv);
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	return kls(argv, argv);
}