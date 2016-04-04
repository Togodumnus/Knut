#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include "../../LIB.h"

int octal_decimal(int n)
{
    int decimal=0, i=0, rem;
    while (n!=0)
    {
        rem = n%10;
        n/=10;
        decimal += rem*pow(8,i);
        ++i;
    }
    return decimal;
}

int chmodLib(int argc, char *argv[]) {
    if (argc>1)
    {
        int droit = atoi(argv[1]);
        droit = octal_decimal(droit);
        int resultat = chmod(argv[2],(mode_t)droit);
        if (resultat==-1)
           {
               printf("Erreur\n");
               return -1;
           }
        return 0;
    }
    return -1;
}

int chmodLib_R(int argc, char *argv[]){
    //TODO
    return 0;
}

/**
 * Init
 *
 * S'enregistre dans le shell dans le cas d'un chargement de la librairie
 * dynamique
 */
void Init(EnregisterCommande enregisterCommande) {
    enregisterCommande("chmod", chmodLib);
}
