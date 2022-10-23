#ifndef SEM_H
#define SEM_H

#define NULL (void *) 0

int initSemSystem();
int semOpen(uint32_t id, uint64_t initialValue);
int semWait(uint32_t id);
int semPost(uint32_t id);
int semClose(uint32_t id);
void printSemInfo();

#endif