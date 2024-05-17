#ifndef HEADER_FILE
#define HEADER_FILE

#include <pthread.h>

#define TRUE 1
#define FALSE 0

typedef struct element {
    int product_id; // Identificador del producto
    int op;         // Operación 0 => PURCHASE; 1 => SALE
    int units;      // Unidades del producto
} element;

typedef struct queue {
  // Define the struct yourself
  int size; // Tamaño de la queue
  int max_size; // Tamaño maximo de la queue
  struct element **array; // Array de punteros a struct
  pthread_mutex_t holding; // Determina si el array esta siendo usado
  pthread_cond_t no_lleno; // Señal que se activa si el array no esta lleno
  pthread_cond_t no_vacio; // Señal que se activa si el array no esta lleno
} queue;


queue *queue_init(int size);
int queue_destroy(queue *q);
int queue_put(queue *q, struct element *elem);
struct element *queue_get(queue *q);
int queue_empty(queue *q);
int queue_full(queue *q);

#endif
