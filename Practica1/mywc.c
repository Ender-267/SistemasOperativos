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

char b[1];
int fd;
int ret;
int bytesRead = -1;
int byteCounter = 0;
int lineCounter = 0;
int wordCounter = 1;
char lastType = '\0'; 

int stringCopy(char str[], char copy[], int size){
	if (copy[size-1] != '\0'){
		return -1;
	}
	for(int i=0;i<size;i++){
		str[i] = copy[i];
	}
	return 0;
}

int debugPrint(int debug){
	if (debug == FALSE){
		return -1;
	}
	else{
		char debugPrint[4];
		char debugPrintType[4];
		switch(b[0]){
			case SPACE:
				if (stringCopy(debugPrint, "SPC", sizeof(debugPrint)) != 0){
					return -2;
				}
				break;
			case TAB:
				if (stringCopy(debugPrint, "TAB", sizeof(debugPrint)) != 0){
					return -2;
				}
				break;
			case NEW_LINE:
				if (stringCopy(debugPrint, "NEW", sizeof(debugPrint)) != 0){
					return -2;
				}
				break;
			default:
				stringCopy(debugPrint, (char[4]){b[0], ' ', ' ', '\0'}, sizeof(debugPrint));
				break;
		}
		switch(lastType){
			case SPACE:
				if (stringCopy(debugPrintType, "SPC", sizeof(debugPrintType)) != 0){
					return -2;
				}
				break;
			case TAB:
				if (stringCopy(debugPrintType, "TAB", sizeof(debugPrintType)) != 0){
					return -2;
				}
				break;
			case NEW_LINE:
				if (stringCopy(debugPrintType, "NEW", sizeof(debugPrintType)) != 0){
					return -2;
				}
				break;
			default:
				stringCopy(debugPrintType, (char[4]){b[0], ' ', ' ', '\0'}, sizeof(debugPrintType));
				break;
		}
		printf("b: %-3d %s l: %-3d w: %-3d t:%s\n", byteCounter, debugPrint, lineCounter, wordCounter ,debugPrintType);
		}
		return 0;
}

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
			switch (lastType){
				case NEW_LINE:
					lineCounter++;
					wordCounter++;
					break;
				case TAB:
					wordCounter++;
				case SPACE:
					wordCounter++;
			}
			lastType = b[0];
		}
		else if (lastType == NEW_LINE){
			if (b[0] != SPACE && b[0] != TAB){
				lineCounter++;
			}
		}
		else{
			lastType = b[0];
		}
		
		/*Debug*/
		debugPrint(TRUE);
	}
	if (b[0] == '\0'){
		wordCounter = 0;
		lineCounter = 0;
	} else if (b[0] == NEW_LINE){
		lineCounter++;
	}
	printf("\n\n");
	printf("%i %i %i\n", lineCounter, wordCounter ,byteCounter);
	int ret = close(fd);
	return ret;
}
