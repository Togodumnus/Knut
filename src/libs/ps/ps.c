#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <ctype.h>


char* REPERTOIRE_PROC = "/proc";

typedef struct{
    char* user;
    char* pid;
}proc;

void print_status(char* path, proc process) {
    char line[100], *p , finalP[256];
    FILE* statusf;

    statusf = fopen(path, "r");
    if(!statusf){
        exit(EXIT_FAILURE);
    }

    while(fgets(line, 100, statusf)) {
        if(strncmp(line, "Uid:", 4) == 0){
            //printf("%s", line);
            // Ignore "Uid:" and whitespace
            p = line + strlen("Uid:");
            while(isspace(*p)) {
                ++p;
            }
            strncpy(finalP,p,strlen(p));
            finalP[strlen(p)+1] = 0;
            process.user = finalP;
            //printf("%s", process.user);  
        }
        if(strncmp(line, "Pid:", 4) == 0){
            //printf("%s\n", line);
            p = line + strlen("Pid:");
            while(isspace(*p)) {
                ++p;
            }
            strncpy(finalP,p,4);
            finalP[4] = 0;
            process.pid = finalP;
            printf("%s", process.pid);
        }
    }

    fclose(statusf);
}


int psNoOpt(int argc, char *argv[]){
    DIR *rep;
    DIR *sousRep;

    struct dirent* fichierLu = NULL; /* Déclaration d'un pointeur vers la structure dirent. */
    struct dirent* fichierLuBis = NULL;

    if ((rep=opendir(REPERTOIRE_PROC))==NULL) {
        printf("Not opened repository\n");
        exit(EXIT_FAILURE);
    }
    printf("repository /proc opened successfully\n");
    
    printf ("USER       PID  %cCPU  %cMEM    VSZ   RSS TTY      STAT START   TIME COMMAND\n",'%','%');

    while ((fichierLu = readdir(rep)) != NULL){
        if (fichierLu->d_type == 4 && atoi(fichierLu->d_name)!=0){ // Si c'est un dossier correspondant aux processus

            // On concatène les chemins pour ouvrir le bon dossier correpsondant
            char* concat_proc_nameOfFile = (char *) malloc(2+strlen(REPERTOIRE_PROC)+strlen(fichierLu->d_name));
            strcpy(concat_proc_nameOfFile, REPERTOIRE_PROC);
            strcat(concat_proc_nameOfFile, "/");
            strcat(concat_proc_nameOfFile, fichierLu->d_name);
            
            if ((sousRep=opendir(concat_proc_nameOfFile))==NULL) {
                printf("Not closed repository\n");
                exit(EXIT_FAILURE);
            }else{
                //On a réussi à ouvrir le dossier correspondant au processus
                while ((fichierLuBis = readdir(sousRep)) != NULL){
                    if (strcmp(fichierLuBis->d_name,"status") == 0){ 
                        proc process;

                        char* path_to_status = (char *) malloc(1+strlen(concat_proc_nameOfFile)+strlen("/status"));
                        strcpy(path_to_status, concat_proc_nameOfFile);
                        strcat(path_to_status, "/status");

                        print_status(path_to_status,process);
                        

                        /*stat(fichierLuBis->d_name, &statProc);
                        struct passwd *user;

                        user = getpwuid(statProc.st_uid); // user name
                        printf(" %s", user->pw_name);*/
                    }   
                }
            }
        }
    }

    if (closedir(rep) == -1){ /* S'il y a eu un souci avec la fermeture */
        exit(EXIT_FAILURE);
    }
    printf("Dossier /proc a ete ferme avec succes\n");


    return 0;

}

int kPs(int argc,char *argv[]){
    int opt = 0;
    while((opt=getopt(argc, argv, "ef"))!=-1)
    switch (opt)
    {
        case 'e':
            //return psLib_e(argc,argv);
            break;
        case 'f':
            //return psLib_f(argc, argv);
            break;
    }
    return psNoOpt(argc,argv);
}


