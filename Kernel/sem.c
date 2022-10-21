#include <lib.h>
#include <memoryManager.h>
#include <processManager.h>
#include <sem.h>
//there are no race conditions in the whole semaphore management because all syscalls block interruptions
//todo mejor manejo de errores

//lista de semaforos
t_sem * semList = NULL;

static t_sem * findSem(uint32_t id);
static t_sem *createSem(uint32_t id, uint64_t initialValue);
static void addSemToList(t_sem * sem);
static void removeSemFromList(t_sem *sem);
t_pid_node * addToBlockedPidsList(int pidToAdd, t_pid_node* list);
t_pid_node *  removeFromBlockedPidsList(int pidToRemove, t_pid_node * list);


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
    removeSemFromList(sem);
  return 0;
}

int semWait(uint32_t id) {
  t_sem * sem = findSem(id);

  if (sem==NULL) return -1;

  if (sem->value > 0){
    sem->value--;
  } else { //bloqueo el proceso
    int callerPID = getpid();
    sem->blockedPidsList = addToBlockedPidsList(callerPID, sem->blockedPidsList);
    blockTask(callerPID);
  }
  return 0;
}

int semPost(uint32_t id) {
  t_sem * sem = findSem(id);

  if (sem == NULL) return -1;

  if (sem->blockedPidsList != NULL){ //desbloqueo el primer pid que quedo bloqueado
    int callerPID = sem->blockedPidsList->pid;
    resumeTask(callerPID);
    sem->blockedPidsList = removeFromBlockedPidsList(callerPID, sem->blockedPidsList);
  } else {
    (sem->value)++;
  }
  return 0;
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
  return sem;
}

//agrega un semaforo al principio de la lista
static void addSemToList(t_sem * sem){
    sem->next = semList;
    semList = sem;
}

static void removeSemFromList(t_sem * sem){
  if(semList == NULL)
    return;

  if(semList->id = sem->id){
    t_sem * aux = semList;
    semList = semList->next;
    free(aux);
    return;
  }

  while(semList->next){
    if(semList->next->id == sem->id){
      t_sem * aux = semList->next;
      semList->next = semList->next->next;
      free(aux);
    }
  }
}

//blockedPidsList functions
//neither add or remove return errors
//both of these functions are tested and work as expected :)

//add pid to the end of list
t_pid_node * addToBlockedPidsList(int pidToAdd, t_pid_node* list){
  t_pid_node * newNode = malloc(sizeof(t_pid_node));
  newNode->pid = pidToAdd;
  newNode->next = NULL;

  if(list == NULL)
    return newNode;

  t_pid_node * auxList = list;
  while(auxList->next)
    auxList = auxList->next;
  auxList->next = newNode;

  return list;
}

t_pid_node *  removeFromBlockedPidsList(int pidToRemove, t_pid_node * list){
  if(list == NULL)
    return list;

  t_pid_node * auxNode;
  if(list->pid == pidToRemove){
    auxNode = list;
    list = auxNode->next;
    free(auxNode);
  } else {
    t_pid_node * auxList = list;
    while(auxList->next != NULL && auxList->next->pid != pidToRemove)
      auxList = auxList->next;
    
    if(auxList->next != NULL){
      auxNode = auxList->next;
      auxList->next = auxList->next->next;
      free(auxNode);
    }
  }
  return list;
}