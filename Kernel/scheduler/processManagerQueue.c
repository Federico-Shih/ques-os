#include <processManagerQueue.h>
#include <stdlib.h>
#include <memoryManager.h>

typedef struct queue_node{
    pcb* pcb;
    struct queue_node *next;
} queue_node;

typedef struct processQueueCDT {
    uint64_t size;
    queue_node *first;
    queue_node *last;
} processQueueCDT;

processQueueADT initQueue() {
  processQueueADT queue = malloc(sizeof(processQueueCDT));
  if (queue == NULL) {
    return queue;
  }
  queue->first = queue->last =  NULL;
  queue->size = 0;
  return queue;
}

uint64_t getQueueSize(processQueueADT queue) {
    return queue->size;
}

pcb* dequeProcess(processQueueADT queue) {
    if (queue->size <= 0)
        return NULL;
    queue_node *aux = queue->first;
    queue->first = queue->first->next;
    queue->size -= 1;
    return aux->pcb;
}

void queueProcess(processQueueADT queue, pcb* process) {
  queue_node* newNode = malloc(sizeof(queue_node));
  newNode->pcb = process;
  newNode->next = NULL;
  if (queue->size == 0) {
    queue->first = newNode;
    queue->last = newNode;
  } else {
    queue->last->next = newNode;
    queue->last = newNode;
  }
}