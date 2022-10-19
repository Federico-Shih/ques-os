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
    queue_node *next;
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
    pcb* res = aux->pcb;
    free(aux);
    return res;
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
  queue->size += 1;
}

pcb* getProcess(processQueueADT queue, int pid)
{
  queue_node* q = queue->first;
  while (q != NULL) 
  {
    if (q->pcb->pid == pid) {
      return q->pcb;
    }
    q = q->next;
  }
  return NULL;
}

int toBegin(processQueueADT queue) 
{
  if( queue->first == NULL)
    return -1;
  queue->next = queue->first;
  return 1;
}

int hasNext(processQueueADT queue) 
{
  return queue->next != NULL;
}

pcb *next(processQueueADT queue)
{
  pcb *aux = queue->next->pcb;
  queue->next = queue->next->next;
  return aux;
}
