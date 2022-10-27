#ifndef SEM_H
#define SEM_H

int initSemSystem();
int semOpen(char *semaphoreName, int initialValue);
int semInit(int initialValue);
int semWait(int id);
int semPost(int id);
int semClose(int id);
void printSemInfo();
void acquire(int *lock);
void release(int *lock);

#endif