//P1-SSOO-23/24

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

struct dirent *lectura_actual;
int fichero_encontrado = FALSE;

int main(int argc, char *argv[])
{
	/* If less than three arguments (argv[0] -> program, argv[1] -> directory to search, argv[2] -> file to find) print an error y return -1 */
	if(argc < 3){
		printf("Too few arguments\n");
		return -1;
	}

	DIR *dir = opendir(argv[1]);
	if (dir == NULL){
		perror("Error de opendir()");
		return -1;
	}
	while(fichero_encontrado == FALSE){
		lectura_actual = readdir(dir);
		if (lectura_actual == NULL){
			break;
		}
		if (strcmp(lectura_actual->d_name, argv[2]) == 0){
			fichero_encontrado = TRUE;
		}
	}
	if (fichero_encontrado){
		printf("File %s is in directory %s\n", argv[2], argv[1]);
	} else{
		printf("File %s is not in directory %s\n", argv[2], argv[1]);
	}
	return 0;
}
