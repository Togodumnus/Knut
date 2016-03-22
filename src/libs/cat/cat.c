#include <stdio.h>
#include <getopt.h>

int catLib(int argc, char *argv[]){
    char buffer[1024];
    FILE *file;
    size_t nread;
    if (argc > 1) {
        for (int i = 1; i < argc; i++){
            file = fopen(argv[i],"r");
            if (file)
            {
                while ((nread = fread(buffer, 1, sizeof buffer, file)) > 0){
                    fwrite(buffer, 1, nread, stdout);
                }
                fclose(file);
            }
            else{
                printf("Problème avec le ou les fichiers\n");
                return -1;
            }
        }
    }
    return 0;
}

int catLib_n(int argc, char *argv[]){
    //TODO
    return 0;
}

int catLib_e(int argc, char *argv[]){
    //TODO
    return 0;
}

int kCatLib(int argc,char *argv[]){
    int opt = 0;
    while((opt=getopt(argc, argv, "en"))!=-1)
    switch (opt)
    {
        case 'e':
            return catLib_e(argc,argv);
            break;
        case 'n':
            return catLib_n(argc, argv);
            break;
    }
    return catLib(argc,argv);
}

