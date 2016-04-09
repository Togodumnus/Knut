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
#include <string.h>

#define NOCOLOR    "\033[1;00m"
#define BLACK    "\033[1;30m"
#define RED      "\033[1;31m"
#define GREEN    "\033[1;32m"
#define YELLOW   "\033[1;33m"
#define BLUE     "\033[1;34m"
#define PURPLE   "\033[1;35m"
#define CYAN     "\033[1;36m"
#define GREY     "\033[1;37m"


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

int kls_no_opt(int argc, char * argv[], int aFlag) {
    return 0;
}

int kls_al(int argc, char * argv[], int aFlag) {
    DIR *dirp;
    struct dirent *dptr;

    struct stat statls;
    struct passwd *user;
    struct group *grd;

    struct tm *tmInfo;

    char path[strlen(argv[argc-1])];


    
    if ((!strcmp(argv[argc-1],"-al"))||(!strcmp(argv[argc-1],"-l"))) { // si y'a pas de chemin donné dans kls
        strcpy(path, "."); // repetoire courant
    }
    else {
        strcpy(path, argv[argc-1]);
    }

    if ((dirp=opendir(path))==NULL) {
        perror("kls");
        exit(EXIT_FAILURE);
    }

    if (argc<4) {
        while ((dptr=readdir(dirp))) {
            // si pas aFlag : on prend pas en compte les fichier cachés etc.. sinon on prend tout
            if (((!aFlag)&&(dptr->d_name[0] != '.'))||(aFlag)) { 
                stat(dptr->d_name, &statls);
        
                if ((statls.st_mode & S_IFSOCK)==S_IFSOCK) printf("s"); // socket
                else if ((statls.st_mode & S_IFLNK)==S_IFLNK) printf("l");  // symbolic link
                else if ((statls.st_mode & S_IFREG)==S_IFREG) printf("-");  // regular file
                else if ((statls.st_mode & S_IFBLK)==S_IFBLK) printf("b");  // block device
                else if ((statls.st_mode & S_IFDIR)==S_IFDIR) printf("d");  // directory
                else if ((statls.st_mode & S_IFCHR)==S_IFCHR) printf("c");  // character device
                else if ((statls.st_mode & S_IFIFO)==S_IFIFO) printf("p");  // FIFO*

                printf("%c",(statls.st_mode & S_IRUSR)==S_IRUSR ? 'r' : '-');   // owner R
                printf("%c",(statls.st_mode & S_IWUSR)==S_IWUSR ? 'w' : '-');   // owner W
                printf("%c",(statls.st_mode & S_IXUSR)==S_IXUSR ? 'x' : '-');   // owner X
                printf("%c",(statls.st_mode & S_IRGRP)==S_IRGRP ? 'r' : '-');   // group R
                printf("%c",(statls.st_mode & S_IWGRP)==S_IWGRP ? 'w' : '-');   // group W
                printf("%c",(statls.st_mode & S_IXGRP)==S_IXGRP ? 'x' : '-');   // group X
                printf("%c",(statls.st_mode & S_IROTH)==S_IROTH ? 'r' : '-');   // other R
                printf("%c",(statls.st_mode & S_IWOTH)==S_IWOTH ? 'w' : '-');   // other W
                printf("%c",(statls.st_mode & S_IXOTH)==S_IXOTH ? 'x' : '-');   // other X

                printf(" %d", (int) statls.st_nlink); // number of hard links

                user = getpwuid(statls.st_uid); // user name
                printf(" %s", user->pw_name);

                grd = getgrgid(statls.st_gid); // group name
                printf(" %s\t", grd->gr_name);

                printf(" %d\t",(int) statls.st_size); // taille

                tmInfo =  localtime(&statls.st_mtime); 
                printf(" %s %d %d:%d\t",mouthName(tmInfo->tm_mon), tmInfo->tm_mday, tmInfo->tm_hour, tmInfo->tm_min);
                if (S_ISDIR(statls.st_mode)) {
                    printf("%s",BLUE);
                    printf(" %s",dptr->d_name);
                    printf("%s",NOCOLOR);
                    printf("/\n");
                }
                else    
                    printf(" %s\n",dptr->d_name);
            }
            

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

    int aFlag, lFlag = 0;

    while((c = getopt(argc, argv, "al")) != -1) {
        switch(c) {
            case 'a':
                aFlag = 1;
                break;
            case 'l':
                lFlag = 1;
                break;
            case '?':
                printf("kls: invalid option -- '%c'\n",c);
                break;
        }
    }
    if (lFlag) {
        return kls_al(argc, argv, aFlag);
    }
    else {
        return kls_no_opt(argc, argv, aFlag);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    return kls(argc, argv);

}