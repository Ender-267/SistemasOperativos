//P1-SSOO-23/24

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define TRUE 1
#define FALSE 0

#define SALTO_LINEA '\n'
#define ESPACIO ' '
#define TAB '\t'

char b[1];
int bytesLeidos = -1;
int contadorBytes = 0;
int contadorLineas = 0;
int contadorPalabras = 0;
char tipoPrevio = '\0'; 

int main(int argc, char *argv[]){
	/*If less than two arguments (argv[0] -> program, argv[1] -> file to process) print an error y return -1*/
	if(argc < 2)
	{
		printf("Argumentos insuficientes\n");
		return -1;
	}
	int fd = open(argv[1], O_RDONLY);
	if (fd == -1){
		printf("Error de open()");
		return -1;
	}
	while(TRUE){
		bytesLeidos = read(fd, b, 1);
		if(bytesLeidos == -1){
			printf("Error de read()");
			return -1;
		}
		else if (bytesLeidos == 0){
			break;
		}
		contadorBytes++;

		if (b[0] != ESPACIO && b[0] != TAB && b[0] != SALTO_LINEA && b[0] != '\0'){
			switch (tipoPrevio){
				case SALTO_LINEA:
					contadorLineas++;
					contadorPalabras++;
					break;
				case TAB:
					contadorPalabras++;
					break;
				case ESPACIO:
					contadorPalabras++;
					break;
				case '\0':
					contadorPalabras++;
					break;
			}
			tipoPrevio = b[0];
		}
		else if (tipoPrevio == SALTO_LINEA){
			if (b[0] != ESPACIO && b[0] != TAB){
				contadorLineas++;
			}
		}
		else{
			tipoPrevio = b[0];
		}
	}
	if (tipoPrevio == SALTO_LINEA){
		contadorLineas++;
	}
	printf("\n\n");
	printf("%i %i %i\n", contadorLineas, contadorPalabras ,contadorBytes);
	int ret = close(fd);
	return ret;
}
