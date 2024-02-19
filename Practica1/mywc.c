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
/*-1 = Fichero vacio, 0 = Normal, 1 = Espacio, 2 = Nueva linea*/
int lastType = -1;
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
		if (b[0] == SPACE){
			lastType = 1;
		}
		else if (b[0] == TAB){
			lastType = 1;
		}
		else if (b[0] == NEW_LINE){
			lastType = 2;
		}
		else {
			if (lastType == 1){
				wordCounter++;
			}
			else if (lastType == 2 || lastType == -1){
				lineCounter++;
				wordCounter++;
			}
			lastType = 0;
		}
		/*Debug*/
		if (b[0] == TAB){
			debugPrint[0] = 'T';
			debugPrint[1] = 'A';
			debugPrint[2] = 'B';
		}
		else if (b[0] == SPACE){
			debugPrint[0] = 'S';
			debugPrint[1] = 'P';
			debugPrint[2] = 'C';
		}
		else if (b[0] == NEW_LINE){
			debugPrint[0] = 'N';
			debugPrint[1] = 'E';
			debugPrint[2] = 'W';
		}
		else {
			debugPrint[0] = b[0];
			debugPrint[1] = ' ';
			debugPrint[2] = ' ';
		}
		if (debugMode == TRUE){
			printf("b: %-3d %s l: %-3d w: %-3d t: %-3d\n", byteCounter, debugPrint, lineCounter, wordCounter, lastType);
		}
		/*Debug*/
	}
	printf("\n\n");
	printf("%i %i %i\n", lineCounter, wordCounter ,byteCounter);
	int ret = close(fd);
	return ret;
}
