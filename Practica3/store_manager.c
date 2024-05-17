#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* definiciones para simulación */
#define MAX_PRODUCTS 5

pthread_mutex_t acceso_fichero; // Controla que varios hilos no muevan el puntero de lectura a la vez
// Guardan el numero de operaciones restantes (segun la primera fila del fichero)
FILE *fichero; // Fichero de productores
queue *q;
pthread_mutex_t acceso_purchases; // Controla el acceso al array de compras
int purchases[MAX_PRODUCTS] = {0}; // Cada indice indica una id de producto
pthread_mutex_t acceso_sales; // Controla el acceso al array de ventas
int sales[MAX_PRODUCTS] = {0}; // Cada indice indica una id de producto

int datos_a_procesar; // Numero al principio del fichero

const int precios_purchase[MAX_PRODUCTS] = {2, 5, 15, 25, 100};
const int precios_sales[MAX_PRODUCTS] = {3, 10, 20, 40, 125};

// Función para leer la siguiente operación del archivo
element *archivo_a_elemento(FILE *file) {
    element *op = malloc(sizeof(element));
    if (op == NULL) {
        fprintf(stderr, "Error: no se pudo asignar memoria para la operación\n");
        return NULL;
    }
    char tipo_operacion[16];
    pthread_mutex_lock(&acceso_fichero);
    int resultado = fscanf(file, "%d %15s %d", &op->product_id, tipo_operacion, &op->units);
    datos_a_procesar--;
    if (resultado != 3) {
        if (feof(file) || datos_a_procesar == 0) {
            datos_a_procesar = 0;
            pthread_mutex_unlock(&acceso_fichero);
            op->op = -1;
            op->product_id = -1;
            op->units = -1;
            return op;
        } else {
            pthread_mutex_unlock(&acceso_fichero);
            fprintf(stderr, "Error: no se pudo leer la operación del archivo\n");
            free(op);
            return NULL;
        }
    }
    pthread_mutex_unlock(&acceso_fichero);
    if (op->product_id < 1 || op->product_id > MAX_PRODUCTS){
        free(op);
        fprintf(stderr, "Product_id no valido\n");
        return NULL;
    }
    if (strcmp("PURCHASE", tipo_operacion) == 0){
        op->op = 0;
    } else if (strcmp("SALE", tipo_operacion) == 0){
        op->op = 1;
    } else {
        fprintf(stderr, "Tipo de operacion invalido\n");
        free(op);
        return NULL;
    }
    return op;
}

// Función del productor
void *producer() {
    element *op;
    while ((op = archivo_a_elemento(fichero)) != NULL) {
        int r = queue_put(q, op);
        if (r == -1){
            fprintf(stderr, "Error de queue_put()");
            pthread_exit(0);
        } else if (r == 1) {
            break;
        }
    }
    pthread_cond_signal(&q->no_lleno);
    pthread_cond_signal(&q->no_vacio);
    pthread_exit(0);
}

// Función del consumidor
void *consumer() {
    element *op;
    while (TRUE) {
        /* Si operaciones_a_insertar es mayor que 0 (los hilos productores siguen introduciendo elementos)
        o la cola contiene elementos, continuamos extrayendo*/
        op = queue_get(q);
        if (op->op == -1){
            break;
        }
        switch(op->op){
            case 0:
                pthread_mutex_lock(&acceso_purchases);
                purchases[op->product_id-1] += op->units;
                pthread_mutex_unlock(&acceso_purchases);
                break;
            case 1:
                pthread_mutex_lock(&acceso_sales);
                sales[op->product_id-1] += op->units;
                pthread_mutex_unlock(&acceso_sales);
                break;
        }
        free(op);
    }
    
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <file> <prods> <cons> <bsize>\n", argv[0]);
        return -1;
    }

    char *entrada = argv[1];
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);
    int buffer_size = atoi(argv[4]);

    if (num_consumers == 0 || num_producers == 0 || buffer_size == 0){
        fprintf(stderr, "Error de parametros\n");
    }

    pthread_t producers[num_producers], consumers[num_consumers];

    if (pthread_mutex_init(&acceso_fichero, NULL) != 0) {
        perror("Error de inicializacion acceso_fichero\n");
        return -1;
    }
    if (pthread_mutex_init(&acceso_sales, NULL) != 0) {
        perror("Error de inicializacion acceso_sales\n");
        return -1;
    }
    if (pthread_mutex_init(&acceso_purchases, NULL) != 0) {
        perror("Error de inicializacion acceso_purchase\n");
        return -1;
    }

    fichero = fopen(entrada, "r");
    if (fichero == NULL) {
        perror("Error abriendo fichero 1\n");
        return -1;
    }

    if (fscanf(fichero, "%d\n", &datos_a_procesar) != 1) {
        perror("No se pudo leer el numero de operaciones\n");
    }
    datos_a_procesar += 2;
    q = queue_init(buffer_size);

    for (int i = 0; i < num_producers; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }
    for (int i = 0; i < num_consumers; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }

    int profits = 0;

    for(int i = 0; i < MAX_PRODUCTS;i++){
        profits += (sales[i]*precios_sales[i] - purchases[i]*precios_purchase[i]);
    }

    printf("Total: %d euros\n", profits);
    printf("Stock:\n");
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        printf("  Product %d: %d\n", i + 1, purchases[i] - sales[i]);
    }
    fclose(fichero);
    pthread_mutex_destroy(&acceso_fichero);
    pthread_mutex_destroy(&acceso_sales);
    pthread_mutex_destroy(&acceso_purchases);
    queue_destroy(q);

    return EXIT_SUCCESS;
}
