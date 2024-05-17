//SSOO-P3 23/24

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"

#define FALSE 0
#define TRUE 1


//To create a queue
queue *queue_init(int size) {
    queue *q = (queue *) malloc(sizeof(queue));
    q->array = calloc(size, sizeof(element));
    q->size = 0;
    q->max_size = size;
    pthread_mutex_init(&q->holding, NULL);
    pthread_cond_init(&q->no_lleno, NULL);
    pthread_cond_init(&q->no_vacio, NULL);
    return q;
}

/* insertar un elemento en la cola */
int queue_put(queue *q, struct element *x) {
    pthread_mutex_lock(&q->holding);
    if (x == NULL) {
        pthread_mutex_unlock(&q->holding);
        return -1;
    }
    while (queue_full(q)) {
        pthread_cond_wait(&q->no_lleno, &q->holding);
    }
    if (x->op == -1){
        // Aseguramos que solo haya un marcador final en la queue
        for(int i = 0; i < q->size;i++){
            if(q->array[i]->op == -1){
                // Sacamos el marcador final anterior de la queue
                for(int j = i; j < q->size - 1; j++) {
                    q->array[j] = q->array[j + 1];
                }
                q->size--;
                break;
            }
        }
        q->array[q->size] = x;
        q->size++;
        pthread_cond_signal(&q->no_vacio);
        pthread_mutex_unlock(&q->holding);
        printf("INSERTADO %d %d %d\n", x->product_id, x->op, x->units);
        return 1;
    }
    q->array[q->size] = x;
    printf("INSERTADO %d %d %d\n", x->product_id, x->op, x->units);
    q->size++;
    pthread_cond_signal(&q->no_vacio);
    pthread_mutex_unlock(&q->holding);

    return 0;
}


// To Dequeue an element.
/* obtener un elemento de la cola */
struct element *queue_get(queue *q) {
    pthread_mutex_lock(&q->holding);
    element *ret = NULL;
    while (queue_empty(q)) {
        pthread_cond_wait(&q->no_vacio, &q->holding);
    }

    if (queue_empty(q)) {
        pthread_mutex_unlock(&q->holding);
        ret->op = -1;
        return ret;
    }
    ret = q->array[0];
    if (q->array[0]->op == -1){
        pthread_cond_signal(&q->no_lleno);
        pthread_mutex_unlock(&q->holding);
        return ret;
    }
    for(int i = 0; i < q->size - 1; i++) {
        q->array[i] = q->array[i + 1];
    }
    q->size--;
    q->array[q->size] = NULL;
    pthread_cond_signal(&q->no_lleno);
    pthread_mutex_unlock(&q->holding);
    return ret;
}


/* verificar si la cola está vacía */
int queue_empty(queue *q) {
    if (q->size <= 0){
        return TRUE;
    } else {
        return FALSE;
    }
}

/* verificar si la cola está llena */
int queue_full(queue *q) {
    if (q->size >= q->max_size){
        return TRUE;
    } else {
        return FALSE;
    }
}

/* destruir la cola */
int queue_destroy(queue *q) {
    pthread_mutex_destroy(&q->holding);
    pthread_cond_destroy(&q->no_lleno);
    pthread_cond_destroy(&q->no_vacio);
    free(q->array);
    free(q);
    return 0;
}
