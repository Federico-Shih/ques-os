#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

typedef struct queueCDT *queueADT;

//creates a new queue
queueADT initQueue();

int freeQueue(queueADT queue);

uint64_t getQueueSize(queueADT queue);
// Remove first element
void* dequeue(queueADT queue);
// Add element to the end
void enqueue(queueADT queue, void* value);
// Shows actual element
void* peek(queueADT queue);
// Resets the queue iterator to the first element.
// 1 if has elements
int toBegin(queueADT queue);
// 
int hasNext(queueADT queue) ;
// 
void *next(queueADT queue);
// 
void *find(queueADT queue, int (*findCondition)(void *, void*), void* element);
// Returns 0 if not found, return 1 if found
int includes(queueADT queue, int (*findCondition)(void *, void*), void* element);

int removeElement(queueADT queue, int (*removeCondition)(void *, void *), void *element);

#endif