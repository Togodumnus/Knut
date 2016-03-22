#include <stdio.h>

int catLib(int argc, char *argv[]) {
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
            }
    	}
    }
    return 0;
}