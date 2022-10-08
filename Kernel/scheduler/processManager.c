#include <processManager.h>
#include <stdlib.h>
#include <interrupts.h>
#include <string.h>

#define STACK_SIZE 0x1024 * 4

static int pidCounter = 1;
static processQueueADT queue = NULL;
static int currentCycles;

static pcb *idleProcessPCB = NULL;

static pcb *currentProcessPCB = NULL;

void idleProcess(int argc, char **argv)
{
  while (1)
    _hlt();
}

void initScheduler()
{
  queue = initQueue();
  if (queue == NULL)
  {
    return;
  }

  char *idleArgv[] = {"idle"};

  initProcess(idleProcess, 1, idleArgv, BACKGROUND_PRIORITY);

  idleProcessPCB = dequeProcess(queue);
}

void *scheduleProcess(void *currStackPointer)
{
}

void *initProcess(void (*process)(int argc, char **argv), int argc, char **argv, priority_type foreground)
{
  
  pcb* newProcess = malloc(sizeof(pcb));
  if (currentProcessPCB != NULL) newProcess->ppid = currentProcessPCB->pid;
  newProcess->pid = pidCounter++;
  newProcess->foreground = foreground;
  newProcess->state = READY;
  if (argv[0] != NULL) {
    // strcpy argv -> newProcess->name
  }
  newProcess->priority = FOREGROUND_PRIORITY;
  newProcess->rsp = newProcess->rbp = malloc(sizeof(char) * STACK_SIZE);
  newProcess->argc = argc;
  newProcess->argv = argv;
  queueProcess(queue, newProcess);
} 

void printProcesses()
{
}

int getpid()
{
  return (currentProcessPCB != NULL) ? currentProcessPCB->pid : 0;
}

void killProcess(int pid)
{
}

void nice(int pid)
{
}

void blockProcess(int pid)
{
}

void resumeProcess(int pid)
{
}

void yield()
{
}