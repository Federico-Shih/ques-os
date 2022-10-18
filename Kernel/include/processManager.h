#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <processManagerQueue.h>
#include <stdint.h>

#define MAX_PRIO 20

void initScheduler();

void *scheduleProcess(void *currStackPointer);

int initProcess(void (*process)(int argc, char** argv), int argc, char** argv, int foreground, int *fd);

void printProcesses();

void printProcess(int pid);

int getpid();

int killProcess(int pid);

void nice(int pid, int priorityLevel);

void blockProcess(int pid);

void resumeProcess(int pid);

void yield();

int cpyArgs(char **dest, char **from, int count);

int changeState(int pid, process_state newState);

char *foregToBool(int foreground);

char *stateToStr(process_state state);

void _callTimerTick();

pcb* initializeBlock(char* name, priority_type foreground, int *fd);
void initializeStack(void (*process)(int, char**), int argc, char **argv, void *rbp);

typedef struct {
  uint64_t gs;
  uint64_t fs;
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t r11;
  uint64_t r10;
  uint64_t r9;
  uint64_t r8;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t rbp;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rbx;
  uint64_t rax;

  uint64_t rip;
  uint64_t cs;
  uint64_t eflags;
  uint64_t rsp;
  uint64_t ss;
  uint64_t base;
} stackFrame;

#endif