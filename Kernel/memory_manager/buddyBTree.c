#include "buddyBTree.h"

void initializeList(list_t *list)
{
  list->prev = list;
  list->next = list;
}

void pushToList(list_t *list, list_t *entry)
{
  list_t *prev = list->prev;
  entry->prev = prev;
  entry->next = list;
  prev->next = entry;
  list->prev = entry;
}

list_t *popFromList(list_t *list)
{
  list_t *back = list->prev;
  if (back == list) return NULL;
  removeFromList(back);
  return back;
}

void removeFromList(list_t *entry)
{
  list_t *prev = entry->prev;
  list_t *next = entry->next;
  prev->next = next;
  next->prev = prev;
}

char listIsEmpty(list_t *list)
{ 
    return list->prev == list; 
}

