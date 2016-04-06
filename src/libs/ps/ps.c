#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <ctype.h>
#include <stdbool.h>

#include "../../LIB.h"

/*TODO

Lien important :
https://www.kernel.org/doc/Documentation/filesystems/proc.txt

TROUVER POURQUOI CERTAINS UID N'ONT PAS DE NOMS D'UTILISATEURS

TROUVER %CPU %MEM TTY

Calcul CPU
http://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat
*/

char* REPERTOIRE_PROC = "/proc";

typedef struct{
    char* user;
    int pid;
    int vsz;
    int rss;
}proc;

void infos_from_stat(char* path, proc* process) {

    /*FILE* statusf;
    char c;
    char *infoStat = "\0";
    char **tabChar = NULL;
    int posTabChar = 0;
    int lenInfoStat = 0;

    statusf = fopen(path, "r");
    if(!statusf){
        exit(EXIT_FAILURE);
    }

    do{
        c = fgetc(statusf);
        if (isspace(c)){
            tabChar[posTabChar] = infoStat;
            posTabChar++;
            infoStat = '\0';
        }else{
            lenInfoStat = strlen(infoStat);
            infoStat[lenInfoStat] = c;
            infoStat[lenInfoStat+1] = '\0';
        }

        printf("%c\n",c);
    }while (!feof(statusf));


    fclose(statusf);*/
}

void infos_from_status(char* path, proc* process) {
    char line[100], *p;
    int tmp = 0, pid=0 ,uid =0, vsz = 0 , rss = 0;
    struct passwd *user;
    bool vmSizeHere = false, vmRssHere = false;

    FILE* statusf;

    statusf = fopen(path, "r");
    if(!statusf){
        exit(EXIT_FAILURE);
    }

    while(fgets(line, 100, statusf)) {
        tmp = 0;
        //On est dans la ligne Uid :
        if(strncmp(line, "Uid:", 4) == 0){
            //On récupère l'uid de la ligne et on le transforme en int dans la variable uid
            p = line + strlen("Uid:");
            while(isspace(*p)) {
                ++p;
            }
            for (int i = 0; i < 5; ++i)
            {
                if (isdigit(p[i])){
                    tmp = p[i] - '0';
                    uid = 10 * uid + tmp;
                }
            }

            //A partir de l'uid, on obtient le nom de l'utilisateur
            if (!(user = getpwuid(uid))){ // user name
                process->user = "?";
            }else{
                process->user = user->pw_name;
            }
        }

        //On est dans la ligne Pid :
        else if(strncmp(line, "Pid:", 4) == 0){

            //On récupère le pid qu'on transforme en int
            p = line + strlen("Pid:");
            while(isspace(*p)) {
                ++p;
            }
            for (int i = 0; i < 5; ++i)
            {
                if (isdigit(p[i])){
                    tmp = p[i] - '0';
                    pid = 10 * pid + tmp;
                }
            }

            //on stocke dans la structure proc le pid
            process->pid = pid;
        }

        //On est dans la ligne VmSize
        else if(strncmp(line, "VmSize:", 4) == 0){
            vmSizeHere = true;
            //On récupère le pid qu'on transforme en int
            p = line + strlen("VmSize:");
            while(isspace(*p)) {
                ++p;
            }
            for (int i = 0; i < 12; ++i)
            {
                if (isdigit(p[i])){
                    tmp = p[i] - '0';
                    vsz = 10 * vsz + tmp;
                }
            }

            //on stocke dans la structure proc le VSZ
            process->vsz = vsz;
        }
        //On est dans la ligne VmRSS
        else if(strncmp(line, "VmRSS:", 4) == 0){
            vmRssHere = true;
            //On récupère le pid qu'on transforme en int
            p = line + strlen("VmRSS:");
            while(isspace(*p)) {
                ++p;
            }
            for (int i = 0; i < 12; ++i)
            {
                if (isdigit(p[i])){
                    tmp = p[i] - '0';
                    rss = 10 * rss + tmp;
                }
            }

            //on stocke dans la structure proc le VSZ
            process->rss = rss;
        }

    }
    //Si la ligne vmSize n'existe pas dans le status
    if (!vmSizeHere){
        process->vsz = 0;
    }
    if (!vmRssHere){
        process->rss = 0;
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

    printf ("USER         PID  %cCPU  %cMEM    VSZ      RSS TTY      STAT START   TIME COMMAND\n",'%','%');

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
                    proc process;
                    if (strcmp(fichierLuBis->d_name,"status") == 0){
                        char* path_to_status = (char *) malloc(1+strlen(concat_proc_nameOfFile)+strlen("/status"));
                        strcpy(path_to_status, concat_proc_nameOfFile);
                        strcat(path_to_status, "/status");

                        infos_from_status(path_to_status,&process);
                        printf("%10s %5d %20d %7d \n",
                                process.user,
                                process.pid,
                                process.vsz,
                                process.rss);
                    }
                    /*else if (strcmp(fichierLuBis->d_name,"stat") == 0){ 
                        char* path_to_stat = (char *) malloc(1+strlen(concat_proc_nameOfFile)+strlen("/stat"));
                        strcpy(path_to_stat, concat_proc_nameOfFile);
                        strcat(path_to_stat, "/stat");

                        infos_from_stat(path_to_stat,&process);
                    }*/

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

/**
 * Init
 *
 * S'enregistre dans le shell dans le cas d'un chargement de la librairie
 * dynamique
 */
void Init(EnregisterCommande enregisterCommande) {
    enregisterCommande("ps", kPs);
}
