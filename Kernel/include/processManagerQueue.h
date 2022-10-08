#ifndef PROCESS_MANAGER_QUEUE_H
#define PROCESS_MANAGER_QUEUE_H
#include <stdint.h>

typedef enum
{
  READY,
  BLOCKED,
  TERMINATED
} process_state;

typedef enum { BACKGROUND_PRIORITY = 1, FOREGROUND_PRIORITY } priority_type;


typedef struct processQueueCDT *processQueueADT;

typedef struct pcb
{
  int pid;
  int ppid;
  int foreground;
  process_state state;
  priority_type priority;
  char name[30];
  int fileDescriptors[2];
  void *rsp;
  void *rbp;
  int argc;
  char **argv;
} pcb;

uint64_t getQueueSize(processQueueADT queue);

pcb *dequeProcess(processQueueADT queue);

void queueProcess(processQueueADT queue, pcb *process);

processQueueADT initQueue();

#endif