#ifndef USERSYSCALLS_H
#define USERSYSCALLS_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "shell.h"
#include "colors.h"
#include "shared.h"
#include "syscodes.h"

extern uint64_t _syscall(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

/* Input ouput */
char read();
int write(const char *buffer, uint64_t size, color_t *colors);
int clear(uint8_t fd);
int showCursor(int active);

/* System */
int inforeg(uint64_t* buffer);
int getRTC(uint64_t id);
int printMem(uint64_t direc, uint8_t* buffer, uint64_t bytes);
int sleep(uint64_t seconds);

/* Memory management */
void* malloc(size_t memory);
void free(void* memory);

/* Process Management */
int startTask(void (*process)(unsigned int argc, char **argv), int argc, char **argv, int foreground, int *fd);
int getpid();
int kill(int pid);
void nice(int pid, int newPriority);
int block(int pid);
int resume(int pid);
void yield();
int waitpid(int pid);

/* Semaphores */
int semOpen(char *semaphoreName, int initialValue);
int semInit(int initialValue);
int semWait(int id);
int semPost(int id);
int semClose(int id);

/* Pipes */
int pipeOpen(int pipeId);
int pipeWrite(int pipeId, char *str);
int pipeClose(int pipeId);
int pipeRead(int pipeId);

/* System info */
userlandPipeInfo * getPipeInfo();
schedulerInfo* getSchedulerInfo();
userlandSemInfo * getSemInfo();
memoryInfo *getMemoryInfo();

#endif