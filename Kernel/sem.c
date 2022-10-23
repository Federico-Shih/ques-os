#include <lib.h>
#include <memoryManager.h>
#include <processManager.h>
#include <sem.h>
#include "queue.h"
//there are no race conditions in the whole semaphore management because all syscalls block interruptions
//todo mejor manejo de errores

typedef struct t_sem {
  queueADT blockedPidsQueue;
  uint16_t  attachedProcesses; //para init y destroy automatico
  uint32_t id;
  uint64_t value;
  int lock;
} t_sem;

//cola de semaforos
queueADT semQueue = NULL;

//necessary to find and delete semaphores from the queue
int *findSemCondition(void * queueElement, void * value){
  return ((t_sem*)queueElement)->id == *((int*) value);
}

static t_sem *findSem(uint32_t id);
static t_sem *createSem(uint32_t id, uint64_t initialValue);

int initSemSystem(){
  if(semQueue != NULL)
    semQueue = initQueue();

  return semQueue != NULL;
}

//si el semaforo no esta creado lo crea
//si ya existia le agrega un attachedProcess
int semOpen(uint32_t id, uint64_t initialValue)
{
  t_sem * sem = findSem(id);

  if (sem == NULL) {
    sem = createSem(id, initialValue);
    if (sem == NULL) {
      return -1;
    }
  }

  sem->attachedProcesses++;
  return id;
}

//si hay muchos procesos attached al semaforo hace attached--
//si es el unico attached cierra el semaforo
int semClose(uint32_t id)
{
  t_sem * sem = findSem(id);
  
  if(sem == NULL) return -1;

  if(sem->attachedProcesses > 0)
    sem->attachedProcesses--;
  else
    removeElement(semQueue, findSemCondition, &id);
  return 0;
}

int semWait(uint32_t id)
{
  t_sem * sem = findSem(id);

  if (sem == NULL) return -1;

  if (sem->value > 0){
    sem->value--;
  } else { //bloqueo el proceso
    int * callerPID = malloc(sizeof(int));
    *callerPID = getpid();
    enqueue(sem->blockedPidsQueue, callerPID);
    blockTask(callerPID);
  }
  return 0;
}

int semPost(uint32_t id)
{
  t_sem * sem = findSem(id);
  if (sem == NULL) return -1;

  //desbloqueo el primer pid que quedo bloqueado
  if (getQueueSize(sem->blockedPidsQueue) > 0){ 
    int * callerPID = dequeue(sem->blockedPidsQueue);
    resumeTask(callerPID);
    free(callerPID);
  } else {
    (sem->value)++;
  }
  return 0;
}


//auxiliary functions

//devuelve un puntero a semaforo a partir de un id
//si no esta devuelve null
t_sem * findSem(uint32_t id)
{
  uint32_t auxId = id;
  return find(semQueue, findSemCondition, &auxId);
}

//crea un nuevo semaforo
static t_sem *createSem(uint32_t id, uint64_t initialValue)
{
  t_sem *  sem = malloc(sizeof(t_sem));
  if (sem != NULL) {
    sem->id = id;
    sem->value = initialValue;
    sem-> attachedProcesses = 0;
    sem->lock = 0;
    sem->blockedPidsQueue = initQueue();
    //agrega el semaforo a la cola
    enqueue(semQueue, sem);
  }
  return sem;
}

//TODO: if a pid is in the middle of the blockedPids list and that pid is killed what happens