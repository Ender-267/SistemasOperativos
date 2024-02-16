//P1-SSOO-23/24

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define TRUE 1
#define FALSE 0

char buffer[1];
int fd;
int ret;
int bytesRead = -1;
int byteCounter = 0;
int lineCounter = 0;
int wordCounter = 0;
int lastType = 0; 
char debug = 0;

int main(int argc, char *argv[])
{
	/*If less than two arguments (argv[0] -> program, argv[1] -> file to process) print an error y return -1*/
	if(argc < 2)
	{
		printf("Too few arguments\n");
		return -1;
	}
	if(argc >= 3){
		char debug = *argv[2];
		printf("DEBUG\n");
	}
	int fd = open(argv[1], O_RDONLY);
	if (fd == -1){
		printf("Open error");
		return -1;
	}
	while(bytesRead != 0){
		bytesRead = read(fd, buffer, 1);
		if(bytesRead == -1){
			printf("Read error");
			return -1;
		}
		//DEBUG
		if(debug=="1"){
			printf("%i    %c\n",byteCounter, buffer[0]);
		}
		switch(buffer[0]){
			case '\n':
				lineCounter++;
				break;
			case ' ':
				wordCounter++;
				break;
			case '\t':
				wordCounter++;
				break;
			default:
				break;
		}
		byteCounter = byteCounter + bytesRead;
	}
	printf("%i %i %i\n", lineCounter, wordCounter ,byteCounter);
	int ret = close(fd);
	return ret;
}

