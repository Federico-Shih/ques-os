#include <stdlib.h>
#include <queue.h>
#include <stdint.h>

typedef struct queue_node
{
  void *value;
  struct queue_node *next;
} queue_node;

typedef struct queueCDT
{
  uint64_t size;
  queue_node *first;
  queue_node *last;
  queue_node *next;
} queueCDT;

queueADT initQueue()
{
  queueADT queue = malloc(sizeof(queueCDT));
  if (queue == NULL)
  {
    return queue;
  }
  queue->first = queue->last = NULL;
  queue->size = 0;
  return queue;
}

// Esta funcion depende de que el valor dentro de tu queue no tenga mallocs anidados.
// Para limpiar mallocs anidados, usar dequeue y limpiarlo vos mismo.
int freeQueue(queueADT queue)
{
  if (queue == NULL)
    return -1;

  void *aux;
  while ((aux = dequeue(queue)) != NULL)
  {
    free(aux);
  }
  free(queue);
  return 0;
}

uint64_t getQueueSize(queueADT queue)
{
  return queue->size;
}

void *dequeue(queueADT queue)
{
  if (queue->size <= 0)
    return NULL;
  queue_node *aux = queue->first;
  queue->first = queue->first->next;
  queue->size -= 1;
  void *res = aux->value;
  free(aux);
  return res;
}

int enqueue(queueADT queue, void *value)
{
  queue_node *newNode = malloc(sizeof(queue_node));
  if (newNode == NULL)
  {
    return -1;
  }
  newNode->value = value;
  newNode->next = NULL;
  if (queue->size == 0)
  {
    queue->first = newNode;
    queue->last = newNode;
  }
  else
  {
    queue->last->next = newNode;
    queue->last = newNode;
  }
  queue->size += 1;
  return 0;
}

void *peek(queueADT queue)
{
  if (queue->size == 0)
    return NULL;
  return queue->first->value;
}

// Si no existe primero
int toBegin(queueADT queue)
{
  if (queue == NULL)
    return -1;
  queue->next = queue->first;
  return 0;
}

int hasNext(queueADT queue)
{
  if (queue == NULL)
    return 0;
  return queue->next != NULL;
}

void *next(queueADT queue)
{
  if (queue == NULL || (!hasNext(queue)))
    return NULL;
  void *aux = queue->next->value;
  queue->next = queue->next->next;
  return aux;
}

void *find(queueADT queue, int (*findCondition)(void *, void *), void *element)
{
  if (queue == NULL || findCondition == NULL)
    return NULL;

  toBegin(queue);
  void *aux;
  while (hasNext(queue))
  {
    aux = next(queue);
    if (findCondition(aux, element))
      return aux;
  }
  return NULL;
}

int includes(queueADT queue, int (*findCondition)(void *, void *), void *element)
{
  return find(queue, findCondition, element) != NULL;
}

int removeElement(queueADT queue, int (*removeCondition)(void *, void *), void *element)
{
  void* poppedElement = popElement(queue, removeCondition, removeElement);
  if (poppedElement != NULL)
  {
    free(poppedElement);
    return 1;
  }
  return 0;
}


void* popElement(queueADT queue, int (*removeCondition)(void *, void *), void *element)
{
  if (queue == NULL || removeCondition == NULL || getQueueSize(queue) == 0) return 0;
  queue_node* node = queue->first;
  queue_node* prev = NULL;
  uint64_t index = 0;
  while (node != NULL)
  {
    if (removeCondition(node->value, element)) 
    {
      if (prev == NULL)
      {
        queue->first = node->next;
      } else
      {
        prev->next = node->next;
      }
      if (index == (getQueueSize(queue) - 1))
      {
        queue->last = prev;
      }
      void* nodeValue = node->value;
      free(node);
      queue->size -= 1;
      return nodeValue;
    }
    index += 1;
    prev = node;
    node = node->next;
  }
  return NULL;
}