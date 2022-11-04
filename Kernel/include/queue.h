#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

typedef struct queueCDT *queueADT;

typedef struct iteratorCDT *iteratorADT;

typedef int (*comparator)(void *, void *);

// Crea una cola generica
queueADT initQueue();

// Libera la cola
int freeQueue(queueADT queue);

// Obtiene la longitud de la cola
uint64_t getQueueSize(queueADT queue);

// Desencola un elemento
void *dequeue(queueADT queue);

// Encola un elemento
int enqueue(queueADT queue, void *value);

// Se fija el valor actual
void *peek(queueADT queue);

// Crea un objeto iterador
iteratorADT toBegin(queueADT queue);

// Devuelve si quedan elementos en el iterador
int hasNext(iteratorADT iterator);

// Devuelve el siguiente valor a iterar
void *next(iteratorADT iterator);

// Obtiene un valor a partir de una condicion
void *find(queueADT queue, comparator func, void *element);

// Obtiene si un valor esta o no en la cola
int includes(queueADT queue, comparator func, void *element);

// Elimina el primer elemento que cumpla con la condicion y lo libera
int removeElement(queueADT queue, comparator func, void *element);

// Elimina el primer elemento que cumpla con la condicion y te lo devuelve
void *takeElement(queueADT queue, comparator func, void *element);

#endif