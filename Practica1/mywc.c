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

int copiar_string(char str[], char copiar[], int size){
	if (copiar[size-1] != '\0'){
		return -1;
	}
	for(int i=0;i<size;i++){
		str[i] = copiar[i];
	}
	return 0;
}

int print_debug(int debug){
	if (debug == FALSE){
		return -1;
	}
	else{
		char print1[4];
		char print2[4];
		switch(b[0]){
			case ESPACIO:
				if (copiar_string(print1, "SPC", sizeof(print1)) != 0){
					return -2;
				}
				break;
			case TAB:
				if (copiar_string(print1, "TAB", sizeof(print1)) != 0){
					return -2;
				}
				break;
			case SALTO_LINEA:
				if (copiar_string(print1, "NEW", sizeof(print1)) != 0){
					return -2;
				}
				break;
			default:
				copiar_string(print1, (char[4]){b[0], ' ', ' ', '\0'}, sizeof(print1));
				break;
		}
		switch(tipoPrevio){
			case ESPACIO:
				if (copiar_string(print2, "SPC", sizeof(print2)) != 0){
					return -2;
				}
				break;
			case TAB:
				if (copiar_string(print2, "TAB", sizeof(print2)) != 0){
					return -2;
				}
				break;
			case SALTO_LINEA:
				if (copiar_string(print2, "NEW", sizeof(print2)) != 0){
					return -2;
				}
				break;
			default:
				copiar_string(print2, (char[4]){b[0], ' ', ' ', '\0'}, sizeof(print2));
				break;
		}
		printf("b: %-3d %s l: %-3d w: %-3d t:%s\n", contadorBytes, print1, contadorLineas, contadorPalabras ,print2);
		}
		return 0;
}

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
		
		/*Debug*/
		print_debug(TRUE);
	}
	if (tipoPrevio == SALTO_LINEA){
		contadorLineas++;
	}
	printf("\n\n");
	printf("%i %i %i\n", contadorLineas, contadorPalabras ,contadorBytes);
	int ret = close(fd);
	return ret;
}
