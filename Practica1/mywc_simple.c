//P1-SSOO-23/24

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define TRUE 1
#define FALSE 0

#define NEW_LINE '\n'
#define SPACE ' '
#define TAB '\t'

int debugMode = TRUE; /*Poner en true para debug*/
/*Uso 0x7F como nulo*/
char b[1] = {0x7F};
int fd;
int ret;
int bytesRead = -1;
int byteCounter = -1;
int lineCounter = 0;
int wordCounter = 1;

int main(int argc, char *argv[]){
	/*If less than two arguments (argv[0] -> program, argv[1] -> file to process) print an error y return -1*/
	if(argc < 2)
	{
		printf("Too few arguments\n");
		return -1;
	}
	int fd = open(argv[1], O_RDONLY);
	if (fd == -1){
		printf("Open error");
		return -1;
	}
	while(bytesRead){
		bytesRead = read(fd, b, 1);
		if(bytesRead == -1){
			printf("Read error");
			return -1;
		}
		byteCounter++;
        switch (b[0]){
		    case SPACE:
                wordCounter++;
                break;
            case TAB:
                wordCounter++;
                break;
            case NEW_LINE:
                lineCounter++;
                wordCounter++;
                break;
            case 0x7F:
                wordCounter = 0;
                lineCounter = 0;
                byteCounter = 0;
                break;
        }		
	}
	printf("%i %i %i %s\n", lineCounter, wordCounter ,byteCounter, argv[1]);
	int ret = close(fd);
	return ret;
}
