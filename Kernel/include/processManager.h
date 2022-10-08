#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <processManagerQueue.h>
#include <stdint.h>

#define MAX_PRIO 20

void initScheduler();

void *scheduleProcess(void *currStackPointer);

void *initProcess(void (*process)(int argc, char** argv), int argc, char** argv, priority_type foreground);

void printProcesses();

int getpid();

void killProcess(int pid);

void nice(int pid);

void blockProcess(int pid);

void resumeProcess(int pid);

void yield();

#endif