#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int kcp(int argc, char * const argv[]) {
	int fd1 = open(argv[1], O_RDONLY);
	int fd2 = open(argv[2], O_WRONLY);

	char buffer[64];
	//int c;

	int nbEcrit =read(fd1,buffer,sizeof(buffer)) ;
	do {	 
		write(fd2,buffer,sizeof(buffer)) ;
		nbEcrit =read(fd1,buffer,sizeof(buffer)) ;
	}while(nbEcrit !=0);

  	close(fd1);
   	close(fd2);
   	return 0;

}

int main(int argc, char * const argv[]) {
	return kcp(argc, argv);
   
}