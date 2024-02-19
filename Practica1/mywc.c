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

char b[1];
int fd;
int ret;
int bytesRead = -1;
int byteCounter = 0;
int lineCounter = 0;
int wordCounter = 0;
char lastType = NULL; 
char debugPrint[3];

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
	while(TRUE){
		bytesRead = read(fd, b, 1);
		if(bytesRead == -1){
			printf("Read error");
			return -1;
		}
		else if (bytesRead == 0){
			break;
		}
		byteCounter++;
		if (b[0] != SPACE && b[0] != TAB && b[0] != NEW_LINE){
			if (lastType == SPACE || lastType == TAB || lastType == NULL){
				wordCounter++;
			}
			else if (lastType == NEW_LINE){
				lineCounter++;
				wordCounter++;
			}
			lastType = 'N';
		}
		else{
			lastType = b[0];
		}
		/*Debug*/
		if (debugMode == TRUE){
			printf("b: %-3d %c l: %-3d w: %-3d t: %-3d\n", byteCounter, b[0], lineCounter, wordCounter, lastType);
		}
		/*Debug*/
	}
	printf("\n\n");
	printf("%i %i %i\n", lineCounter, wordCounter ,byteCounter);
	int ret = close(fd);
	return ret;
}
