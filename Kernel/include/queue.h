#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

typedef struct queueCDT *queueADT;

typedef int (*comparator)(void*, void*);

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
void *find(queueADT queue, comparator func, void* element);

// Returns 0 if not found, return 1 if found
int includes(queueADT queue, comparator func, void* element);

// Removes first element from array that fulfills removeCondition
int removeElement(queueADT queue, comparator func, void *element);

// Removes first element from array that fulfills removeCondition and returns
void* popElement(queueADT queue, comparator func, void *element);


#endif