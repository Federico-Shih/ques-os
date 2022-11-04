#ifndef SEM_H
#define SEM_H
#include "shared.h"

int initSemSystem();
int semOpen(char *semaphoreName, int initialValue);
int semInit(int initialValue);
int semValue(int id);
int semWait(int id);
int semPost(int id);
int semClose(int id);
void printSemInfo();
void acquire(int *lock);
void release(int *lock);

userlandBlockedPids * getBlockedPids(int semId);
userlandSem * getSingleSem(int semId);
userlandSemInfo * getSemInfo();

#endif