#ifndef BUDDYBTREE_H
#define BUDDYBTREE_H

#include <stdlib.h>
#include <stdint.h>

typedef struct list_t
{
  struct list_t *prev, *next;
  uint8_t order;
  // 0 esta libre, 1 esta usado
  uint8_t free;
} list_t;

void initializeList(list_t *list);

void pushToList(list_t *list, list_t *entry);

list_t *popFromList(list_t *list);

void removeFromList(list_t *entry);

char listIsEmpty(list_t *list);

#endif