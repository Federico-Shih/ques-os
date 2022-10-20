#include <lib.h>
#include <memoryManager.h>
#include <processManager.h>
#include <sem.h>

//lista de semaforos
t_sem * semList = NULL;

static t_sem * findSem(uint32_t id);
static t_sem *createSem(uint32_t id, uint64_t initialValue);
static void addSemToList(t_sem * sem);
static void destroySem(t_sem *sem);
static void unblockSemProcess(t_sem *sem);
static void blockedProcessesDump(int *blockedProcesses,
                 uint16_t blockedProcessesAmount);


//si el semaforo no esta creado lo crea
//si ya existia le agrega un attachedProcess
int semOpen(uint32_t id, uint64_t initialValue) {
  t_sem * sem = findSem(id);

  if (sem == NULL) {
    sem = createSem(id, initialValue);
    if (sem == NULL) {
      return -1;
    }
  }

   sem-> attachedProcesses++;
  return id;
}

//si hay muchos procesos attached al semaforo hace attached--
//si es el unico attached cierra el semaforo
int semClose(uint32_t id){
  t_sem * sem = findSem(id);
  
  if(sem == NULL) return -1;

  if(sem->attachedProcesses > 0)
    sem->attachedProcesses--;
  else
    destroySem(sem);
}


//auxiliary functions

//devuelve un puntero a semaforo a partir de un id
//si no esta devuelve null
t_sem * findSem(uint32_t id){
  t_sem * semListAux = semList;
  while(semListAux){
    if (semListAux->id == id)
      return semListAux;
    semListAux->next;
  }
    
  return semListAux;
}


//crea un nuevo semaforo
static t_sem *createSem(uint32_t id, uint64_t initialValue) {
  t_sem *  sem = malloc(sizeof(t_sem));
  if (sem != NULL) {
     sem->id = id;
     sem->value = initialValue;
     sem-> attachedProcesses = 0;
     sem->lock = 0;
     sem->next = NULL;
     sem->blockedPidsList = NULL;
    addSemToList(sem);
  }
  return  sem;
}

//agrega un semaforo al principio de la lista
static void addSemToList(t_sem * sem){
    sem->next = semList;
    semList = sem;
}