//P2-SSOO-23/24

//  MSH main file
// Write your msh source code here

//#include "parser.h"
#include <stddef.h>            /* NULL */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMANDS 8
#define MAX_BKG 16 // Numero maximo de procesos en background

#define TRUE 1
#define FALSE 0


// files in case of redirection
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param) {
    printf("****  Exiting MSH **** \n");
    //signal(SIGINT, siginthandler);
    exit(0);
}

struct command {
    // Store the number of commands in argvv
    int num_commands;
    // Store the number of arguments of each command
    int *args;
    // Store the commands
    char ***argvv;
    // Store the I/O redirection
    char filev[3][64];
    // Store if the command is executed in background or foreground
    int in_background;
};

int history_size = 5;
struct command *history;
int head = 0;
int tail = 0;
int n_elem = 0;

void free_command(struct command *cmd) {
    if ((*cmd).argvv != NULL) {
        char **argv;
        for (; (*cmd).argvv && *(*cmd).argvv; (*cmd).argvv++) {
            for (argv = *(*cmd).argvv; argv && *argv; argv++) {
                if (*argv) {
                    free(*argv);
                    *argv = NULL;
                }
            }
        }
    }
    free((*cmd).args);
}

void store_command(char ***argvv, char filev[3][64], int in_background, struct command *cmd) {
    int num_commands = 0;
    while (argvv[num_commands] != NULL) {
        num_commands++;
    }

    for (int f = 0; f < 3; f++) {
        if (strcmp(filev[f], "0") != 0) {
            strcpy((*cmd).filev[f], filev[f]);
        } else {
            strcpy((*cmd).filev[f], "0");
        }
    }

    (*cmd).in_background = in_background;
    (*cmd).num_commands = num_commands - 1;
    (*cmd).argvv = (char ***) calloc((num_commands), sizeof(char **));
    (*cmd).args = (int *) calloc(num_commands, sizeof(int));

    for (int i = 0; i < num_commands; i++) {
        int args = 0;
        while (argvv[i][args] != NULL) {
            args++;
        }
        (*cmd).args[i] = args;
        (*cmd).argvv[i] = (char **) calloc((args + 1), sizeof(char *));
        int j;
        for (j = 0; j < args; j++) {
            (*cmd).argvv[i][j] = (char *) calloc(strlen(argvv[i][j]), sizeof(char));
            strcpy((*cmd).argvv[i][j], argvv[i][j]);
        }
    }
}


/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char ***argvv, int num_command) {
    //reset first
    for (int j = 0; j < 8; j++)
        argv_execvp[j] = NULL;

    int i = 0;
    for (i = 0; argvv[num_command][i] != NULL; i++)
        argv_execvp[i] = argvv[num_command][i];
}

/* mycalc */

// Función principal para procesar el comando mycalc
void mycalc(char *operando1, char *operacion, char *operando2) {
    int op1 = atoi(operando1);
    int op2 = atoi(operando2);
    int resultado = 0;
    int resto = 0;
    char *op = NULL;
    if (setenv("Acc", "0", 0) == -1){ // Variable de entorno
        perror("Error de setenv()");
        exit(-1);
    } 

    if (strcmp(operacion, "add") == 0) {
        resultado = op1 + op2;
        op = "+";
        char str[32];
        sprintf(str, "%d", atoi(getenv("Acc")) + resultado);
        if (setenv("Acc", str, 1) == -1){ // Variable de entorno
        perror("Error de setenv()");
        exit(-1);
        } 
    } else if (strcmp(operacion, "mul") == 0) {
        resultado = op1 * op2;
        op = "*";
    } else if (strcmp(operacion, "div") == 0) {
        if (op2 != 0) {
            resto = op1 % op2;
            resultado = op1 / op2;
        } else {
            resultado = 0;
        }
        op = "/";

    } else {
        printf("[ERROR] La estructura del comando es mycalc <operando_1> <add/mul/div> <operando_2>\n");
        exit(-1);
    }

    if (strcmp(operacion, "div") == 0) {
        fprintf(stderr, "[OK] %d %s %d = %d; Resto %d\n", op1, op, op2, resultado, resto);
    } else if (strcmp(operacion, "add") == 0){
        fprintf(stderr, "[OK] %d %s %d = %d; Acc %s\n", op1, op, op2, resultado, getenv("Acc"));
    } else {
        fprintf(stderr, "[OK] %d %s %d = %d\n", op1, op, op2, resultado);
    }
}

/* mycalc */

/* myhistory */
void myhistory(){
    for(int i=0; i < n_elem; i++){
        fprintf(stderr, "%d ", i);
        // For loop i itera por los elementos del historial
        struct command cmd = history[i];
        char ***argvv = cmd.argvv;
        for(int j=0;j < cmd.num_commands;j++){
            // For loop j itera por los mandatos del comando
            for(int k=0; k < (cmd.args)[j];k++){
                // For loop k itera por los argumentos de cada mandato
                char *argumento = *(argvv[j] + k);
                fprintf(stderr ,"%s ", argumento);
            }
            if (j < cmd.num_commands - 1){
                fprintf(stderr, "| ");
            }
        }
        if (cmd.filev[0][0] != '0'){
            fprintf(stderr, "< %s ", cmd.filev[0]);
        }
        if (cmd.filev[1][0] != '0'){
            fprintf(stderr, "> %s ", cmd.filev[1]);
        }
        if (cmd.filev[2][0] != '0'){
            fprintf(stderr, "!> %s ", cmd.filev[2]);
        }
        fprintf(stderr, "\n");
    }
}
/* myhistory */

/**
 * Main sheell  Loop
 */
int main(int argc, char *argv[]) {
    /* Inicializaciones del estudiante*/
    unsigned int num_proc_bkg = 0; // Numero de procesos en background
    pid_t lista_proc_bkg[MAX_BKG + 1] = {0}; // Listado de procesos en background
    struct command *copia = NULL; // Struc auxiliar para duplicar un argvv
    int no_guardar = FALSE;



    /**********************************/

    /**** Do not delete this code.****/
    int end = 0;
    int executed_cmd_lines = -1;
    char *cmd_line = NULL;
    char *cmd_lines[10];

    if (!isatty(STDIN_FILENO)) {
        cmd_line = (char *) malloc(100);
        while (scanf(" %[^\n]", cmd_line) != EOF) {
            if (strlen(cmd_line) <= 0) return 0;
            cmd_lines[end] = (char *) malloc(strlen(cmd_line) + 1);
            strcpy(cmd_lines[end], cmd_line);
            end++;
            fflush(stdin);
            fflush(stdout);
        }
    }

    /*********************************/

    char ***argvv = NULL;
    int num_commands;

    history = (struct command *) malloc(history_size * sizeof(struct command));
    int run_history = 0;
    int command_counter = 0;
    int in_background = 0;

    while (1) {
        int status = 0;
        signal(SIGINT, siginthandler);
        if (run_history) {
            run_history = 0;
            argvv = copia->argvv;
            for(int i = 0; i < 3; i++){
                strcpy(filev[i], copia->filev[i]);
            }
            no_guardar = TRUE;
        } else {
            command_counter = 0;
            in_background = 0;
            // Prompt
            write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

            // Get command
            //********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
            executed_cmd_lines++;
            if (end != 0 && executed_cmd_lines < end) {
                command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
            } else if (end != 0 && executed_cmd_lines == end)
                return 0;
            else
                command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
        }
        ///************************************************************************************************

        // Comprobacion de procesos background acabados
        for (int i = 0; i < num_proc_bkg; i++) {
            pid_t pid_act = lista_proc_bkg[i];
            if (pid_act > 0) {
                pid_t cerrado = waitpid(pid_act, NULL, WNOHANG);
                if (cerrado > 0) {
                    for (int j = i; j < num_proc_bkg - 1; j++) {
                        lista_proc_bkg[j] = lista_proc_bkg[j + 1];
                    }
                    num_proc_bkg--;
                    i--; // Decrement i to recheck the current index
                }
            }
        }
        if (command_counter <= 0){
            // Si no hay comandos, continuamos
            continue;
        }
        
        // Mandatos internos
        if (strcmp(*(argvv[0]), "myhistory")  == 0){
            if (*(*argvv + 1) == NULL){
                myhistory();
                continue;
            } else {
                int n;
                if(strcmp(*(*argvv + 1), "0") == 0){
                    n = 0;
                } else {
                    n = atoi(*(*argvv + 1));
                    if (n == 0){
                        perror("Error de atoi()");
                    }
                }
                if (n >= n_elem || n < 0){
                    printf("ERROR: Comando no encontrado\n");
                    continue;
                } else {
                    fprintf(stderr, "Ejecutando el comando %d\n", n);
                    struct command cmd;
                    in_background = history[n].in_background;
                    command_counter = history[n].num_commands;
                    copia = &cmd;
                    argvv = history[n].argvv;
                    argvv[command_counter] = '\0';

                    char filev[3][64];
                    for(int i = 0; i < 3; i++){
                        strcpy(filev[i], history[n].filev[i]);
                    }
                    for(int i = 0; i < command_counter;i++){
                        argvv[i][history[n].args[i]] = '\0';
                    }   
                    store_command(argvv, filev, in_background, copia);
                    run_history = TRUE;
                    continue;
                }
            }
        } else {
            // Guardar historial de comandos
            if (no_guardar == FALSE){
                if (n_elem < history_size){
                    n_elem++;
                } else {
                    free_command(&history[0]);
                    for(int i = 0; i < history_size - 1; i++){
                        history[i] = history[i+1];
                    }
                }
                struct command comando_actual;
                store_command(argvv, filev, in_background, &comando_actual);
                history[n_elem-1] = comando_actual;
            } else {
                no_guardar = FALSE;
            }
            }
        if (strcmp(*(argvv[0]), "fg") == 0) {
            // Traer proceso a foreground
            if (*(*argvv + 1) == NULL){
                fprintf(stderr, "La estructura del comando es fg <job_id>\n");
            } else {
                int job_id = atoi(*(*argvv + 1));
                waitpid(lista_proc_bkg[job_id - 1], NULL, 0);
                for (int i = (job_id - 1); i < num_proc_bkg - 1; i++) {
                    lista_proc_bkg[i] = lista_proc_bkg[i + 1];
                }
            }
            num_proc_bkg--;
        } else if (strcmp(*(argvv[0]), "jobs") == 0){
            // Listar procesos en background
            int job_id;
            for(int i = 0; i < num_proc_bkg; i++){
                job_id = i + 1;
                printf("[%d] %d\n",job_id ,lista_proc_bkg[i]);
            }
        } else if (strcmp(*(argvv[0]), "mycalc") == 0){
            // Mycalc
            if (*(*argvv + 1) == NULL || *(*argvv + 2) == NULL || *(*argvv + 3) == NULL){
                printf("[ERROR] La estructura del comando es mycalc <operando_1> <add/mul/div> <operando_2>\n");
            } else {
                mycalc(*(*argvv + 1), *(*argvv + 2), *(*argvv + 3));
            }
            
        } else {
            // Mandatos externos
            // Creacion de pipes
            int pipe_entrada[2];
            int pipe_salida[2];
            if (pipe(pipe_entrada) == -1){
                perror("Error de pipe()\n");
            }
            if (pipe(pipe_salida) == -1){
                perror("Error de pipe()\n");
            }
            for(int i=0;i<command_counter;i++){
                // Comandos
                char* comando = *(argvv[i]);
                char** argumentos = argvv[i];
                pid_t pid = fork();
                if (pid < 0) {
                    // Error de fork
                    perror("Error de fork()\n");
                } else if (pid == 0) {
                    // Proceso hijo
                    // Control de redireciones
                    if (strcmp(filev[0], "0") != 0 && i == 0){
                            int fd = open(filev[0], O_RDONLY);
                            if (fd == -1){
                                perror("Error de open()\n");
                                exit(-1);
                            }
                            if (dup2(fd, STDIN_FILENO) == -1){
                                perror("Error de dup2()\n");
                                exit(-1);
                            }
                            if (close(fd) == -1){
                                perror("Error de close()\n");
                                exit(-1);
                            }
                        }
                    if (strcmp(filev[1], "0") != 0 && i == command_counter-1){
                            int fd = open(filev[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            if (fd == -1){
                                perror("Error de open()\n");
                                exit(-1);
                            }
                            if (dup2(fd, STDOUT_FILENO) == -1){
                                perror("Error de dup2()\n");
                                exit(-1);
                            }
                            if (close(fd) == -1){
                                perror("Error de close()\n");
                                exit(-1);
                            }
                        }
                    if (strcmp(filev[2], "0") != 0){
                            int fd = open(filev[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            if (fd == -1){
                                perror("Error de open()\n");
                                exit(-1);
                            }
                            if (dup2(fd, STDERR_FILENO) == -1){
                                perror("Error de dup2()\n");
                                exit(-1);
                            }
                            if (close(fd) == -1){
                                perror("Error de close()\n");
                                exit(-1);
                            }
                        }
                    // Control de pipes
                    if (command_counter>1){
                        if (i == 0) {
                            close(pipe_entrada[0]);
                            close(pipe_entrada[1]);
                            close(pipe_salida[0]);
                            if (dup2(pipe_salida[1], STDOUT_FILENO) == -1) {
                                perror("Error de dup2()\n");
                            }
                            close(pipe_salida[1]);
                        } else if (i == command_counter - 1) {
                            close(pipe_salida[0]);
                            close(pipe_salida[1]);
                            close(pipe_entrada[1]);
                            if (dup2(pipe_entrada[0], STDIN_FILENO) == -1) {
                                perror("Error de dup2()\n");
                            }
                            close(pipe_entrada[0]); 
                        } else {
                            close(pipe_entrada[1]);
                            close(pipe_salida[0]);
                            if (dup2(pipe_entrada[0], STDIN_FILENO) == -1) {
                                perror("Error de dup2()\n");
                            }
                            close(pipe_entrada[0]);
                            if (dup2(pipe_salida[1], STDOUT_FILENO) == -1) {
                                perror("Error de dup2()\n");
                            }
                            close(pipe_salida[1]);
                        }
                    }
                    if (num_proc_bkg >= MAX_BKG && in_background == TRUE) {
                        fprintf(stderr, "Demasiados procesos en background\n");
                        exit(-1);
                    }
                    // Mandatos externos
                    execvp(comando, argumentos);
                    perror("Error de execvp()\n");
                    exit(-1);
                }
                // Proceso padre
                if (close(pipe_entrada[0]) == -1){
                    perror("Error de close()\n");
                }
                if (close(pipe_entrada[1]) == -1){
                    perror("Error de close()\n");
                }
                pipe_entrada[0] = pipe_salida[0];
                pipe_entrada[1] = pipe_salida[1];
                if (pipe(pipe_salida) == -1){
                    perror("Error de pipe()\n");
                }
                if (in_background == FALSE) {
                    waitpid(pid, NULL, 0);
                } else {
                    if (i == (command_counter -1)){
                        printf("%d\n", pid);
                    }
                    lista_proc_bkg[num_proc_bkg] = pid;
                    num_proc_bkg++;
                }
            } // Final de for-loop de fork
            if (close(pipe_entrada[0]) == -1){
                    perror("Error de close()\n");
            }
            if (close(pipe_salida[0]) == -1){
                    perror("Error de close()\n");
            }
            if (close(pipe_entrada[1]) == -1){
                    perror("Error de close()\n");
            }
            if (close(pipe_salida[1]) == -1){
                    perror("Error de close()\n");
            }

            /************************ STUDENTS CODE ********************************/
        }
    }
    return 0;
}