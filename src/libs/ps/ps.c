#define _BSD_SOURCE
#define _XOPEN_SOURCE 700
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
#include <time.h>

#include "../../DEBUG.h"
#include "../../LIB.h"

/*TODO

Lien important :
https://www.kernel.org/doc/Documentation/filesystems/proc.txt

TROUVER %CPU %MEM TTY

Calcul CPU
http://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat
*/

const int SLEEP_TIME = 1000000; //10ms
#define MAJOR_OF(d) ( ((unsigned)(d)>>8u) & 0xfffu )

char* REPERTOIRE_PROC = "/proc";

/**
 * proc
 *
 * Représente un processus
 */
typedef struct proc{
    char*  user;
    int    pid;
    int    vsz;
    int    rss;
    float cpu;
    char* tty;
    char* command;
    struct tm timeOf;
} proc;


/**
 * systemStat
 *
 * Quelques infos du système
 */
typedef struct systemStat {
    long unsigned int   totalCpuTime;
    long                clock;          //hertz
} systemStat;


/**
 * freeProc
 *
 * free() ce qu'il faut bien
 *
 * @param  {proc *} p
 */
void freeProc(proc *p) {
    free(p->command);
}


/**
 * split
 *
 * @param  {char *}     str
 * @param  {char *}     delim
 * @param  {int *}      nb      nombre de string lues
 *
 * @return {char **}    Tableau de strings (à free à la fin)
 */
char **split(char *str, char *delim, int *nb) { //pas utilisée

    char **res = (char **) malloc(sizeof(char *));
    int n = 0;

    char *p = strtok(str, delim);
    while (p) {

        res[n] = p;
        n++;
        res = (char **) realloc(res, (n + 1) * sizeof(char *));

        if (res == NULL) {
            perror("realloc error");
            exit(EXIT_FAILURE);
        }

        p = strtok(NULL, delim);
    }

    if (nb != NULL) {
        *nb = n;
    }

    return res;
}

/**
 * getSystemStat
 *
 * Lecture de quelques paramètres système en début de ps
 *
 * @param  {systemStat *}   sysStat     Où sauver les données
 */
void getSystemStat(systemStat *sysStat) { //pas utilisée
    sysStat->clock = sysconf(_SC_CLK_TCK);

    FILE *statf = fopen("/proc/stat", "r");
    if(!statf){
        perror("Can't open status");
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t n = NULL;

    if (getline(&line, &n, statf) == -1) {
        perror("Can't read file");
        exit(EXIT_FAILURE);
    }

    int nb = 0;
    char **items = split(line, " ", &nb);

    sysStat->totalCpuTime = 0;
    for (int i = 1; i < 5; i++) {
        sysStat->totalCpuTime += atoi(items[i]);
    }


    free(items);
    free(line);

    fclose(statf);
}

/**
 * getProcStat
 *
 * Parsing du fichier stat correspondant à un processus
 *
 * @param  {char *} path    Le chemin vers le fichier stat
 * @param  {proc *} process La structure à remplir
 */
void getProcStat(char* path,proc* process) { //pas utilisée
    FILE *statf = fopen(path, "r");
    if(!statf){
        perror("Can't open status");
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t n = NULL;

    if (getline(&line, &n, statf) == -1) {
        perror("Can't read file");
        exit(EXIT_FAILURE);
    }

    int nb = 0;
    char **items = split(line, " ", &nb);

    int tty = atoi(items[6]); // On a l'entier correspondant au tty du process

/*
Comment récupérer le tty : 
The /proc/<pid>/stat files give the tty number for each process (e.g.
34817). I then go through the /dev/pts directory and stat() each file
there.  I then compare one of the struct stat members (st_rdev) returned
by stat() to the tty number found in /proc/<pid>/stat.  If they match,
then the file stated (e.g. /dev/pts/1) is the terminal that that process
is attached to.
*/


    //-----------PARTIE POUR TROUVER LES TTY ---------
    DIR *processRep;
    struct dirent *fileTTY;
    struct stat statTTY;

    char* tty_tmp = "?";
    bool tty_found = false;


    //Tous les tty dans /dev/pts
    if ((processRep = opendir("/dev/pts")) == NULL) {
        printf("Not closed repository\n");
        exit(EXIT_FAILURE);
    } else {
        while ((fileTTY = readdir(processRep))) {
            char path_to_tty[
                1 + strlen("/dev/pts/")
                + strlen(fileTTY->d_name)
                ];
            strcpy(path_to_tty, "/dev/pts/");
            strcat(path_to_tty, fileTTY->d_name);
            if (stat(path_to_tty, &statTTY) == -1) {
                perror("stat error");
                exit(EXIT_FAILURE);
            }
            if (tty == (int)statTTY.st_rdev && 
                strcmp(fileTTY->d_name,".") != 0 &&
                strcmp(fileTTY->d_name,"..") != 0    
                ){
                char* tty_name = malloc(
                    1 + 
                    strlen("pts/") + 
                    strlen (fileTTY->d_name));
                strcpy(tty_name, "pts/");
                strcat(tty_name, fileTTY->d_name);
                tty_tmp = tty_name;
                tty_found = true;
            }
        }
    }


    //Tous les tty dans /dev
    if ((processRep = opendir("/dev")) == NULL) {
        printf("Not closed repository\n");
        exit(EXIT_FAILURE);
    } else if (!tty_found){
        while ((fileTTY = readdir(processRep))) {
            char path_to_tty[
                1 + strlen("/dev/")
                + strlen(fileTTY->d_name)
                ];
            strcpy(path_to_tty, "/dev/");
            strcat(path_to_tty, fileTTY->d_name);
            if (stat(path_to_tty, &statTTY) == -1) {
                perror("stat error");
                exit(EXIT_FAILURE);
            }
            if (tty == (int)statTTY.st_rdev && 
                strncmp(fileTTY->d_name,"tty",3) == 0){
                tty_tmp = fileTTY->d_name;
            }
        }
    }

    process->tty = tty_tmp;


//--------------------FIN DE LA PARTIE POUR LES TTY---------
//--------------------TROUVER LE TIME-----------------------

    int utime = atoi(items[13]);
    int stime = atoi(items[14]);

    long hertz = sysconf(_SC_CLK_TCK);

    int totalSeconds = (utime+stime)/hertz;
    
    int seconds = totalSeconds % 60; 
    int minutes = (totalSeconds / 60) % 60; 
    int hours = totalSeconds / 3600; 

    struct tm timeOf;

    timeOf.tm_hour=hours;
    timeOf.tm_min=minutes;
    timeOf.tm_sec=seconds;

    process->timeOf = timeOf; 


    free(items);
    free(line);

    fclose(statf);
    
}

/**
 * infos_from_status
 *
 * Parsing du fichier status correspondant à un processus
 *
 * @param  {char *} path    Le chemin vers le fichier status
 * @param  {proc *} process La structure à remplir
 */
void infos_from_status(char* path, proc* process) {

    char line[100];

    FILE* statusf;

    statusf = fopen(path, "r");
    if(!statusf){
        perror("Can't open status");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, 100, statusf)) {
        char *token;

        //Name
        if(strncmp(line, "Name:", 4) == 0){
            token = strtok(line, "\t"); //"Name:"
            token = strtok(NULL, "\t");

            char *name = (char *) malloc((strlen(token) + 1) * sizeof(char));
            memset(name, '\0', strlen(token) + 1);
            strcpy(name, token);

            //on stocke dans la structure proc le Name de la commande
            process->command = name;
        }
        //Uid
        else if(strncmp(line, "Uid:", 4) == 0){
            token = strtok(line, "\t"); //"Uid:"
            token = strtok(NULL, "\t"); //l'uid

            int uid = atoi(token);
            struct passwd *user;

            //A partir de l'uid, on obtient le nom de l'utilisateur
            if (!(user = getpwuid(uid))){
                process->user = "?";
            } else {
                process->user = user->pw_name;
            }
        }

        //On est dans la ligne Pid :
        else if(strncmp(line, "Pid:", 4) == 0){
            token = strtok(line, "\t"); //"Pid:"
            token = strtok(NULL, "\t"); //le pid

            //on stocke dans la structure proc le pid
            process->pid = atoi(token);
        }

        //On est dans la ligne VmSize
        else if(strncmp(line, "VmSize:", 4) == 0){
            token = strtok(line, "\t"); //"VmSize:"
            token = strtok(NULL, "\t");

            //on stocke dans la structure proc le VSZ
            process->vsz = atoi(token);
        }
        //On est dans la ligne VmRSS
        else if(strncmp(line, "VmRSS:", 4) == 0){
            token = strtok(line, "\t"); //"VmSize:"
            token = strtok(NULL, "\t");

            //on stocke dans la structure proc le VSZ
            process->rss = atoi(token);
        }
        //On est dans la ligne Name

    }

    fclose(statusf);
}


/**
 * ps
 *
 * Affichage des processus
 *
 * @param  {int}    options
 */
void ps(int options) {

    DIR *rep;
    DIR *processRep;

    struct dirent* fichierLu = NULL;
    struct dirent* fichierLuBis = NULL;


    if ((rep=opendir(REPERTOIRE_PROC))==NULL) {
        printf("Not opened repository\n");
        exit(EXIT_FAILURE);
    }
    DEBUG("repository /proc opened successfully");

    /*systemStat sysStat = {0};*/
    /*getSystemStat(&sysStat);*/

    printf("PID\tTTY\tTIME\tCMD\t\n");

    while ((fichierLu = readdir(rep)) != NULL){
        //Si c'est un dossier correspondant aux processus
        if (fichierLu->d_type == 4 && atoi(fichierLu->d_name) != 0){

            //On concatène les chemins pour ouvrir le bon dossier correspondant
            char processDir[
                2 + strlen(REPERTOIRE_PROC) + strlen(fichierLu->d_name)
            ];
            strcpy(processDir, REPERTOIRE_PROC);
            strcat(processDir, "/");
            strcat(processDir, fichierLu->d_name);

            //on ouvre un processus
            if ((processRep = opendir(processDir)) == NULL) {
                printf("Not closed repository\n");
                exit(EXIT_FAILURE);
            } else { //On a réussi à ouvrir le dossier correspondant au process
                proc process = {0};

                while ((fichierLuBis = readdir(processRep)) != NULL) {


                    if (strcmp(fichierLuBis->d_name, "status") == 0) {
                        char path_to_status[
                            1 + strlen(processDir)
                            + strlen("/status")
                        ];
                        strcpy(path_to_status, processDir);
                        strcat(path_to_status, "/status");

                        infos_from_status(path_to_status, &process);

                    } else if (strcmp(fichierLuBis->d_name, "stat") == 0){
                        char path_to_stat[
                            1 + strlen(processDir)
                            + strlen("/stat")
                        ];
                        strcpy(path_to_stat, processDir);
                        strcat(path_to_stat, "/stat");

                        getProcStat(path_to_stat, &process);
                        printf(
                            "%5d   %6s  %2d:%2d:%2d  %s ",
                            process.pid,
                            process.tty,
                            process.timeOf.tm_hour, 
                            process.timeOf.tm_min, 
                            process.timeOf.tm_sec,
                            process.command
                        );
                    }
                }
                freeProc(&process);

            }
        }
    }

    if (closedir(rep) == -1){ /* S'il y a eu un souci avec la fermeture */
        perror("Can't close dir");
        exit(EXIT_FAILURE);
    }

    DEBUG("Dossier /proc a ete ferme avec succes");

}

int kPs(int argc,char *argv[]){
    int options = 0;

    int opt = 0;
    while ((opt=getopt(argc, argv, "ef"))!=-1) {
        switch (opt) {
            case 'e':
                //TODO
                break;
            case 'f':
                //TODO
                break;
        }
    }

    ps(options);

    return 0;
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
