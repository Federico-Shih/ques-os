#include "lib.h"
#include "memoryManager.h"
#include "processManager.h"
#include "sem.h"
#include "queue.h"
#include "console.h"
#include "string.h"

typedef struct t_sem
{
  queueADT blockedPidsQueue;
  uint16_t attachedProcesses; // para init y destroy automatico
  int id;
  int value;
  int lock;
  char *name;
} t_sem;

int semId = 0;

// cola de semaforos
queueADT semQueue = NULL;

// Usado para prevenir acceso mutuo al crear y destruir semaforos
int allSemsLock = 0;

static t_sem *findSem(int id);
static t_sem *findSemName(char *name);
static t_sem *createSem(int initialValue, char *semName);
static int destroySem(t_sem *semaphore);
extern int _xchg(int *lock, int value);
static void printBlockedSemInfo(queueADT queue);
int findSemCondition(void *queueElement, void *value);
static int getNextSemaphoreId();
int findSemNameCondition(char *queueElement, char *value);

// devuelve != 0 si funciono, 0 si hubo error
int initSemSystem()
{
  acquire(&allSemsLock);
  if (semQueue == NULL)
    semQueue = initQueue();
  release(&allSemsLock);

  return semQueue != NULL;
}

// si el semaforo no esta creado lo crea
// si ya existia le agrega un attachedProcess
int semOpen(char *semaphoreName, int initialValue)
{
  t_sem *sem = findSemName(semaphoreName);

  acquire(&allSemsLock);
  if (sem == NULL)
  {
    sem = createSem(initialValue, semaphoreName);
    if (sem == NULL)
    {
      release(&allSemsLock);
      return -1;
    }
  }

  sem->attachedProcesses++;
  release(&allSemsLock);
  return sem->id;
}

int semInit(int initialValue)
{
  acquire(&allSemsLock);
  t_sem *sem = createSem(initialValue, "");
  release(&allSemsLock);
  if (sem == NULL)
  {
    return -1;
  }
  return sem->id;
}

// Cierra o destruye un semaforo. Si es unnamed se destruye, si es named se fija si sigue teniendo procesos
// escuchando al semaforo
int semClose(int id)
{
  t_sem *sem = findSem(id);

  if (sem == NULL)
    return -1;

  acquire(&allSemsLock);
  if (strlen(sem->name) == 0 || sem->attachedProcesses == 1)
  {
    t_sem *closedSemaphore = popElement(semQueue, findSemCondition, &id);
    destroySem(closedSemaphore);
  }
  else
    sem->attachedProcesses--;

  release(&allSemsLock);
  return 0;
}

int semWait(int id)
{
  t_sem *sem = findSem(id);

  if (sem == NULL)
    return -1;

  acquire(&(sem->lock));
  if (sem->value > 0)
  {
    sem->value--;
    release(&(sem->lock));
  }
  else
  { // bloqueo el proceso
    int *callerPID = malloc(sizeof(int));
    *callerPID = getpid();
    enqueue(sem->blockedPidsQueue, callerPID);
    release(&(sem->lock));
    blockTask(*callerPID);
  }
  return 0;
}

int semPost(int id)
{
  t_sem *sem = findSem(id);
  if (sem == NULL)
    return -1;

  // desbloqueo el primer pid que quedo bloqueado.
  acquire(&(sem->lock));
  while (getQueueSize(sem->blockedPidsQueue) > 0)
  {
    int *callerPID = dequeue(sem->blockedPidsQueue);
    int pid = resumeTask(*callerPID);
    free(callerPID);
    // Se pudo desbloquear
    if (pid != -1)
    {
      release(&(sem->lock));
      (sem->value)++;
      return 0;
    }
  }
  (sem->value)++;
  release(&(sem->lock));
  return 0;
}

int semValue(int id)
{
  t_sem *sem = findSem(id);
  if (sem == NULL)
    return -1;
  return sem->value;
}

void printSemInfo()
{ // todo race conditions aca
  if (toBegin(semQueue) != 0)
  {
    printf("No hay semaforos\n");
    return;
  }
  while (hasNext(semQueue))
  {
    t_sem *sem = (t_sem *)next(semQueue);
    printf("Id del semaforo: %d\n", sem->id);
    printf("Valor del semaforo: %d\n", (int)sem->value);
    printf("Cantidad de procesos vinculados: %d\n", sem->attachedProcesses);
    printf("Lock del semaforo: %d\n", sem->lock);
    printf("Procesos bloqueados: \n");
    printBlockedSemInfo(sem->blockedPidsQueue);
    printf("\n\n");
  }
}

static void printBlockedSemInfo(queueADT queue)
{
  if (toBegin(queue) != 0)
    printf("No hay procesos bloqueados. \n");
  while (hasNext(queue))
  {
    int *pid = (int *)next(queue);
    printf("Id del proceso bloqueado: %d\n", *pid);
  }
}

// ----------------------- AUXILIARY FUNCTIONS ---------------------------------------------

// devuelve un puntero a semaforo a partir de un id
// si no esta devuelve null
static t_sem *findSem(int id)
{
  int auxId = id;
  return find(semQueue, findSemCondition, &auxId);
}

static t_sem *findSemName(char *name)
{
  return find(semQueue, (comparator)findSemNameCondition, (void *)name);
}

// necessary to find and delete semaphores from the queue
int findSemCondition(void *queueElement, void *value)
{
  return ((t_sem *)queueElement)->id == *((int *)value);
}

int findSemNameCondition(char *queueElement, char *value)
{
  return strcasecmp(queueElement, value);
}

// crea un nuevo semaforo
static t_sem *createSem(int initialValue, char *name)
{
  t_sem *sem = malloc(sizeof(t_sem));
  if (sem != NULL)
  {
    sem->id = getNextSemaphoreId();
    sem->value = initialValue;
    sem->attachedProcesses = 0;
    sem->lock = 0;
    sem->blockedPidsQueue = initQueue();
    sem->name = malloc((strlen(name) + 1) * sizeof(char));
    if (sem->name == NULL)
    {
      free(sem);
      return NULL;
    }
    strcpy(sem->name, name);
    // agrega el semaforo a la cola
    enqueue(semQueue, sem);
  }
  return sem;
}

// elimina un semaforo
static int destroySem(t_sem *semaphore)
{
  freeQueue(semaphore->blockedPidsQueue);
  free(semaphore->name);
  free(semaphore);
  return 0;
}

static int getNextSemaphoreId()
{
  return semId++;
}

// ----------------------------------- LOCK RELATED FUNCTIONS ------------------------------------------------
void acquire(int *lock)
{
  while (_xchg(lock, 1) != 0)
    ;
}

void release(int *lock)
{
  _xchg(lock, 0);
}
// -----------------------------------------------------------------------------------------------------------
