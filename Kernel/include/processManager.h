#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stdint.h>
#include "queue.h"
#include "sem.h"
#include "shared.h"

#define MAX_PRIO 20

// La diferencia entre TERMINATED y EXITED es que TERMINATED puede ser liberado y EXITED tiene que ser waiteado.
// Un proceso permanentemente en EXITED es un proceso huerfano.
typedef enum
{
  READY,
  BLOCKED,
  EXITED,
  TERMINATED,
} process_state;

typedef enum { BACKGROUND_PRIORITY = 1, FOREGROUND_PRIORITY } priority_type;

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
  int semId;
} pcb;


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

// Obtiene un proceso en especifico
pcb* getProcess(queueADT queue, int pid);

queueADT getProcessQueue();

pcb* getCallingProcess();

// Inicializa el scheduler y la queue
void initScheduler();

/*
  Decide que hacer para el proceso actual corriendo.
  Se encarga de liberar los procesos que estan terminados
*/
void *scheduleTask(void *currStackPointer);

/*
Empieza un proceso. 
Args:
- Puntero a la funcion del proceso
- argc y argv
- Si es foreground o background
- File descriptors. Si se envia NULL, hereda los filedescriptors del padre.
*/ 
int startTask(void (*process)(int argc, char** argv), int argc, char** argv, int foreground, int *fd);

schedulerInfo* getSchedulerInfo();

// Obtiene el pid del proceso actual
int getpid();

int killTask(int pid);

int nice(int pid, int priorityLevel);

int blockTask(int pid);

int resumeTask(int pid);

int killCurrent();

int killCurrentForeground();

int yield();

int cpyArgs(char **dest, char **from, int count);

// Cambia de estado a un proceso. Devuelve el pid del proceso que se cambio o -1 si el proceso no existe.
int changeState(int pid, process_state newState);

char *foregToBool(int foreground);

char *stateToStr(process_state state);

void _callTimerTick();

pcb* initializeBlock(char* name, int foreground, int *fd);

void initializeStack(void (*process)(int, char**), int argc, char **argv, void *rbp);

int waitpid(int pid);

int setUserlandPid(int pid);

#endif