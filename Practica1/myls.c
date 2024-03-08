//P1-SSOO-23/24

#include <stdio.h>		// Header file for system call printf
#include <unistd.h>		// Header file for system call gtcwd
#include <sys/types.h>	// Header file for system calls opendir, readdir y closedir
#include <dirent.h>
#include <string.h>
#include <linux/limits.h>  //PATH_MAX

#define TRUE 1
#define FALSE 0

char path_actual[PATH_MAX];
struct dirent* lectura_actual;

int main(int argc, char *argv[]){
	if (argc > 1) {
		/*Guardamos el path en una variable*/
        strcpy(path_actual, argv[1]);
    } else{
		/*Si no definimos un path en los parametros del programa, getcwd() recoge el path*/
        if (getcwd(path_actual, PATH_MAX) == NULL){
			perror("Error de getcwd()");
			return -1;
		}
	}
	/*Abrimos el directorio definido por el path*/
	DIR *dir = opendir(path_actual);

	if (dir == NULL){
		perror("Error de opendir()");
		return -1;
	}

	while(TRUE){
		/*Leemos los subdirectorios*/
		lectura_actual = readdir(dir);

        if (lectura_actual == NULL) {
            break;
        };
		/*Imprimimos el nombre de los subdirectorios con cada iteracion del bucle*/
		printf("%s\n", lectura_actual->d_name);
	};
	int ret = closedir(dir);
	return ret;
}

