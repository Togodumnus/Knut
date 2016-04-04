#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>


int psLib(int argc, char *argv[]){
    DIR *rep;
    struct dirent* fichierLu = NULL; /* Déclaration d'un pointeur vers la structure dirent. */


    if ((rep=opendir("/proc"))==NULL) {
        printf("Répertoire pas ouvert\n");
        return -1;
    }
    printf("Dossier /proc ouvert avec succès\n");

    while ((fichierLu = readdir(rep)) != NULL){
        printf("Le fichier lu s'appelle '%s'\n", fichierLu->d_name);
    }

    if (closedir(rep) == -1){ /* S'il y a eu un souci avec la fermeture */
        return -1;
    }
    printf("Dossier /proc a ete ferme avec succes\n");


    return 0;

}

int kPsLib(int argc,char *argv[]){
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
    return psLib(argc,argv);
}


