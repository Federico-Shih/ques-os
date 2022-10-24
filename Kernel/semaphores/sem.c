#include "lib.h"
#include "memoryManager.h"
#include "processManager.h"
#include "sem.h"
#include "queue.h"
#include <console.h>
//there are no race conditions in the whole semaphore management because all syscalls block interruptions
//todo mejor manejo de errores

extern int _xchg(int *lock, int value);
static void printBlockedSemInfo(queueADT queue);

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
int findSemCondition(void * queueElement, void * value){
  return ((t_sem*)queueElement)->id == *((int*) value);
}

static t_sem *findSem(uint32_t id);
static t_sem *createSem(uint32_t id, uint64_t initialValue);

// Usado para prevenir acceso mutuo al crear y destruir semaforos
int allSemsLock = 0;

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

  acquire(&allSemsLock);
  if (sem == NULL) {
    sem = createSem(id, initialValue);
    if (sem == NULL) {
      return -1;
    }
  }

  sem->attachedProcesses++;
  release(&allSemsLock);
  return id;
}

//si hay muchos procesos attached al semaforo hace attached--
//si es el unico attached cierra el semaforo
int semClose(uint32_t id)
{
  t_sem * sem = findSem(id);
  
  if(sem == NULL) return -1;

  acquire(&allSemsLock);
  if(sem->attachedProcesses > 0)
    sem->attachedProcesses--;
  else
    removeElement(semQueue, findSemCondition, &id);
  release(&allSemsLock);
  return 0;
}

int semWait(uint32_t id)
{
  t_sem * sem = findSem(id);

  if (sem == NULL) return -1;

  acquire(&(sem->lock));
  if (sem->value > 0){
    sem->value--;
    release(&(sem->lock));
  } else { //bloqueo el proceso
    int * callerPID = malloc(sizeof(int));
    *callerPID = getpid();
    enqueue(sem->blockedPidsQueue, callerPID);
    release(&(sem->lock));
    blockTask(*callerPID);
  }
  return 0;
}

int semPost(uint32_t id)
{
  t_sem * sem = findSem(id);
  if (sem == NULL) return -1;

  //desbloqueo el primer pid que quedo bloqueado.
  acquire(&(sem->lock));
  while (getQueueSize(sem->blockedPidsQueue) > 0) {
    int * callerPID = dequeue(sem->blockedPidsQueue);
    int pid = resumeTask(*callerPID);
    free(callerPID); 
    // Se pudo desbloquear
    if (pid != -1)
    {
      release(&(sem->lock));
      return 0;
    }
  }
  (sem->value)++;
  release(&(sem->lock));
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

// Lock and unlock
void acquire(int *lock){
  while(_xchg(lock, 1) != 0);
}

void release(int *lock){
  _xchg(lock, 0);
}

void printSemInfo()
{
  if(toBegin(semQueue) != 0)
  {
    printf("No hay semaforos\n");
    return;
  }
  while(hasNext(semQueue))
  {
    t_sem *sem = (t_sem *)next(semQueue);
    printf("Id del semaforo: %d\n", sem->id);
    printf("Valor del semaforo: %d\n", sem->value);
    printf("Cantidad de procesos vinculados: %d\n", sem->attachedProcesses);
    printf("Lock del semaforo: %d\n",sem->lock);
    printf("Procesos bloqueados: \n");
    printBlockedSemInfo(sem->blockedPidsQueue);
    printf("\n\n");
  }
}

static void printBlockedSemInfo(queueADT queue)
{
  if(toBegin(queue) != 0)
    printf("No hay procesos bloqueados. \n");
  while(hasNext(queue))
  {
    int *pid = (int *)next(queue);
    printf("Id del proceso bloqueado: %d\n", *pid);
  }
} 

