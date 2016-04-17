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
    char*  tty;
    char*  command;
    int    sec;
    int    min;
    int    hour;
} proc;

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
 * getProcStat
 *
 * Parsing du fichier stat correspondant à un processus
 *
 * @param  {char *} path    Le chemin vers le fichier stat
 * @param  {proc *} process La structure à remplir
 */
void getProcStat(char* path,proc* process) { //pas utilisée

    char *line = NULL; //le fichier (1 ligne)
    size_t n = 0;

    //ouverture
    FILE *statf = fopen(path, "r");
    if (!statf){
        perror("Can't open status");
        exit(EXIT_FAILURE);
    }

    //lecture
    if (getline(&line, &n, statf) == -1) {
        perror("Can't read file");
        exit(EXIT_FAILURE);
    }

    char **items = split(line, " ", NULL);

    /*
     * Calcul du tty
     */

    int tty = atoi(items[6]); // On a l'entier correspondant au tty du process
    DIR *processRep;
    struct dirent *fileTTY;
    struct stat statTTY;

    char* tty_tmp = "?";
    bool tty_found = false;

    //Tous les tty de /dev/pts
    if ((processRep = opendir("/dev/pts")) == NULL) {
        perror("Can't open repository");
        exit(EXIT_FAILURE);
    } else {
        while ((fileTTY = readdir(processRep))) {

            //on prend des infos sur le fichier
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

            //Si le tty correspnd
            if (tty == (int) statTTY.st_rdev
                    && strcmp(fileTTY->d_name, ".")  != 0
                    && strcmp(fileTTY->d_name, "..") != 0
                ) {

                //on retiens le tty
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

        closedir(processRep);
    }

    if (!tty_found) {

        //Même chose que tous les tty, mais dans /dev
        if ((processRep = opendir("/dev")) == NULL) {
            perror("Can't open repository");
            exit(EXIT_FAILURE);
        } else {
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

                if (tty == (int) statTTY.st_rdev
                        && strncmp(fileTTY->d_name,"tty",3) == 0
                    ) {
                    tty_tmp = fileTTY->d_name;
                }
            }

            closedir(processRep);
        }
    }

    process->tty = tty_tmp;

    /*
     * Calcul du time
     */

    int utime  = atoi(items[13]);
    int stime  = atoi(items[14]);
    long hertz = sysconf(_SC_CLK_TCK);

    int totalSeconds = (utime + stime) / hertz;

    DEBUG("totalSeconds = %d", totalSeconds);

    process->sec  = totalSeconds % 60;
    process->min  = (totalSeconds / 60) % 60;
    process->hour = totalSeconds / 3600;

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
    if (!statusf){
        perror("Can't open status");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, 100, statusf)) {
        char *token;

        //Name
        if(strncmp(line, "Name:", 4) == 0){
            strtok(line, "\t"); //"Name:"
            token = strtok(NULL, "\t");

            int l = strlen(token) + 1;
            char *name = (char *) malloc(l * sizeof(char));
            strcpy(name, token);
            name[l - 1] = '\0';

            process->command = name;
        }
        //Uid
        else if(strncmp(line, "Uid:", 4) == 0){
            strtok(line, "\t"); //"Uid:"
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
            strtok(line, "\t"); //"Pid:"
            token = strtok(NULL, "\t"); //le pid

            process->pid = atoi(token);
        }
        //On est dans la ligne VmSize
        else if(strncmp(line, "VmSize:", 4) == 0){
            strtok(line, "\t"); //"VmSize:"
            token = strtok(NULL, "\t");

            process->vsz = atoi(token);
        }
        //On est dans la ligne VmRSS
        else if(strncmp(line, "VmRSS:", 4) == 0){
            strtok(line, "\t"); //"VmSize:"
            token = strtok(NULL, "\t");

            process->rss = atoi(token);
        }
    }

    fclose(statusf);
}


/**
 * ps
 *
 * Fonction principale
 * Affichage des processus "à la ps -e"
 */
void psE() {

    DIR *rep;
    DIR *processRep;

    if ((rep = opendir(REPERTOIRE_PROC)) == NULL) {
        perror("Can't open process repository");
        exit(EXIT_FAILURE);
    }
    DEBUG("repository /proc opened successfully");

    printf("    PID\t   TTY\t   TIME\t   CMD\t\n ");

    struct dirent* dossierProcess = NULL;
    while ((dossierProcess = readdir(rep)) != NULL){
        //Si c'est un dossier correspondant aux processus
        if (dossierProcess->d_type == 4 && atoi(dossierProcess->d_name) != 0){

            //On concatène les chemins pour ouvrir le bon dossier correspondant
            char processDirFileName[
                2 + strlen(REPERTOIRE_PROC) + strlen(dossierProcess->d_name)
            ];
            strcpy(processDirFileName, REPERTOIRE_PROC);
            strcat(processDirFileName, "/");
            strcat(processDirFileName, dossierProcess->d_name);

            //on ouvre un processus
            if ((processRep = opendir(processDirFileName)) == NULL) {
                perror("Can't open process repository");
                exit(EXIT_FAILURE);
            } else {
                //On a réussi à ouvrir le dossier correspondant au process
                proc process = {0};

                struct dirent* fichierInfoProcess = NULL;
                while ((fichierInfoProcess = readdir(processRep)) != NULL) {

                    //on ouvre /proc/[pid]/status
                    if (strcmp(fichierInfoProcess->d_name, "status") == 0) {
                        char path_to_status[
                            1 + strlen(processDirFileName)
                            + strlen("/status")
                        ];
                        strcpy(path_to_status, processDirFileName);
                        strcat(path_to_status, "/status");

                        infos_from_status(path_to_status, &process);

                    //on ouvre /proc/[pid]/stat
                    } else if (strcmp(fichierInfoProcess->d_name, "stat") == 0){
                        char path_to_stat[
                            1 + strlen(processDirFileName)
                            + strlen("/stat")
                        ];
                        strcpy(path_to_stat, processDirFileName);
                        strcat(path_to_stat, "/stat");

                        getProcStat(path_to_stat, &process);
                    }
                }

                printf(
                    "%5d   %6s  %02d:%02d:%02d  %6s ",
                    process.pid,
                    process.tty,
                    process.hour,
                    process.min,
                    process.sec,
                    process.command
                );

                freeProc(&process);
            }
        }
    }

    if (closedir(rep) == -1){
        perror("Can't close dir");
        exit(EXIT_FAILURE);
    }

    DEBUG("Dossier /proc a ete ferme avec succes");
}

/**
 * kPs
 *
 */
int kPs(int argc, char *argv[]) {

    psE();
    return 0;
}

