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
int bytesLeidos = -1;
int contadorBytes = -1;
int contadorLineas = 0;
int contadorPalabras = 1;

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
	while(bytesLeidos){
		bytesLeidos = read(fd, b, 1);
		if(bytesLeidos == -1){
			printf("Read error");
			return -1;
		}
		contadorBytes++;
        switch (b[0]){
		    case ESPACIO:
                contadorPalabras++;
                break;
            case TAB:
                contadorPalabras++;
                break;
            case SALTO_LINEA:
                contadorLineas++;
                contadorPalabras++;
                break;
            case '\0':
                contadorPalabras = 0;
                contadorLineas = 0;
                contadorBytes = 0;
                break;
        }		
	}
	printf("%i %i %i %s\n", contadorLineas, contadorPalabras ,contadorBytes, argv[1]);
	int ret = close(fd);
	return ret;
}
