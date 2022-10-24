#ifndef SEM_H
#define SEM_H

#include <stdlib.h>

int initSemSystem();
int semOpen(uint32_t id, uint64_t initialValue);
int semWait(uint32_t id);
int semPost(uint32_t id);
int semClose(uint32_t id);
void printSemInfo();
void acquire(int *lock);
void release(int *lock);

#endif