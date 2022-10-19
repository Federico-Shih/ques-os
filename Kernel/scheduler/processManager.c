#include <processManager.h>
#include <stdlib.h>
#include <interrupts.h>
#include <string.h>
#include <console.h>

#define STACK_SIZE (1024 * 4)

static int pidCounter = 1;
static processQueueADT queue = NULL;
static int currentCycles;

// idle process
static pcb *idleProcessPCB = NULL;

// current process
static pcb *currentProcessPCB = NULL;

// queue ready processes
static int readyCount = 0;

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

  startTask(idleProcess, 1, idleArgv, 0, NULL);

  readyCount -= 1;
  idleProcessPCB = dequeProcess(queue);
}

void freeProcess(pcb* process) {
  for (int i = 0; i < process->argc; i += 1) {
    free(process->argv[i]);
  }
  free(process->argv);
  // Previamente se hizo malloc en el stack del proceso
  free((void *)((char *)process->rbp - STACK_SIZE + 1));
  free((void *)process);
}

// Tengo que buscar que proceso correr
void *scheduleTask(void *currStackPointer)
{
  if (queue == NULL) return currStackPointer;
  // Se fija si hay un proceso actual en existencia. Si no lo hay no hay cambio de contexto.
  if (currentProcessPCB != NULL) {
    // Intento correrlo y descuento la cantidad de ciclos
    if (currentProcessPCB->state == READY && currentCycles > 0) {
      currentCycles -= 1;
      return currStackPointer;
    }

    // Si se quedo sin ciclos para correr, guardarlo.
    currentProcessPCB->rsp = currStackPointer;

    // Si el proceso actual es el idle ignorarlo.
    if (currentProcessPCB->pid != idleProcessPCB->pid) {
      // Libero el proceso actual si esta terminado
      if (currentProcessPCB->state == TERMINATED) {
        // Reactivo el padre si el hijo actual estaba en foreground
        pcb* parent = getProcess(queue, currentProcessPCB->ppid);
        if (parent != NULL && currentProcessPCB->foreground &&  parent->state == BLOCKED) {
          changeState(parent->pid, READY);
        }
        freeProcess(currentProcessPCB);
      } else {
        queueProcess(queue, currentProcessPCB);
      }
    } else {
      idleProcessPCB = currentProcessPCB;
    }
  }

  if (readyCount > 0) 
  {
    currentProcessPCB = dequeProcess(queue);

    // Como readyCount > 0, me aseguro que existe algun proceso
    while (currentProcessPCB->state != READY) {
      if (currentProcessPCB->state == TERMINATED) 
      {
        freeProcess(currentProcessPCB);
      }
      if (currentProcessPCB->state == BLOCKED)
      {
        queueProcess(queue, currentProcessPCB);
      }
      currentProcessPCB = dequeProcess(queue);
    }  
  } 
  else 
  {
    currentProcessPCB = idleProcessPCB;
  }
  
  currentCycles = currentProcessPCB->priority;
  return currentProcessPCB->rsp;
}

int startTask(void (*process)(int argc, char **argv), int argc, char **argv, int foreground, int *fd)
{
  if (process == NULL) return -1;
  
  pcb* newProcess = initializeBlock(argv[0], foreground, NULL);
  
  if (newProcess == NULL) return -1;

  char **args = malloc(sizeof(char *) * argc);
  
  if (cpyArgs(args, argv, argc) == -1) {
    freeProcess(newProcess);
    return -1;
  }

  newProcess->argc = argc;
  newProcess->argv = args;

  initializeStack(process, argc, args, newProcess->rbp);
  
  queueProcess(queue, newProcess);
  readyCount += 1;
  // Bloqueo el padre.
  if (newProcess->foreground && newProcess->ppid) {
    blockTask(newProcess->ppid);
  }

  return newProcess->pid;
} 

void printTasks()
{
  pcb *aux;
  toBegin(queue);
  while(hasNext(queue)){
    aux = next(queue);
    printTask(aux->pid);
  }
}

void printTask(int pid)
{
  pcb *process = getProcess(queue, pid);
  if( process != NULL )
  {
    printf(
      "Nombre: %s\nPID: %d\nPPID: %d\nForeground: %s\nRSP: %x\nRBP: "
        "%x\nPrioridad: %d\nEstado: %s\n\n",
        process->name, process->pid, process->ppid,
        foregToBool((int)process->foreground), (uint64_t)process->rsp,
        (uint64_t)process->rbp, process->priority,
        stateToStr(process->state));
  }
}

char *foregToBool(int foreground)
{
  return foreground ? "TRUE" : "FALSE";
}

char *stateToStr(process_state state)
{
  switch (state)
  {
    case READY:
      return "READY";
    case BLOCKED:
      return "BLOCKED";
    case TERMINATED:
      return "TERMINATED";
    default:
      return "TROYAN HORSE INCOMING!";
  }
}

int getpid()
{
  return (currentProcessPCB != NULL) ? currentProcessPCB->pid : 0;
}

int killTask(int pid)
{
  int id = changeState(pid, TERMINATED);

  if(id == currentProcessPCB->pid)
    _callTimerTick();

  return id;
}

void nice(int pid, int priorityValue)
{
  if( priorityValue < 0)
    priorityValue = 0;
  if( priorityValue > MAX_PRIO)
    priorityValue = MAX_PRIO;

  pcb *process = getProcess(queue, pid);   

  if(process != NULL) {
    process->priority = priorityValue;
  }  
}

void blockTask(int pid)
{
  int id = changeState(pid, BLOCKED);
  if( id == currentProcessPCB->pid)
    _callTimerTick();
}

void resumeTask(int pid)
{
  changeState(pid, READY);
}

void yield()
{
  currentCycles = 0;
  _callTimerTick();
}

void killCurrent() {
  killTask(currentProcessPCB->pid);
}

static void processWrapper(void (*process)(int, char**), int argc, char **argv) 
{
  process(argc, argv);
  killCurrent();
}


void initializeStack(void (*process)(int, char**), int argc, char **argv, void *rbp)
{
  // Ubico el stackframe correctamente
  stackFrame *sf = (stackFrame *)rbp - 1;
  
  // Cargo el stackframe inicial
  sf->gs = 0x001;
  sf->fs = 0x002;
  sf->r15 = 0x003;
  sf->r14 = 0x004;
  sf->r13 = 0x005;
  sf->r12 = 0x006;
  sf->r11 = 0x007;
  sf->r10 = 0x008;
  sf->r9 = 0x009;
  sf->r8 = 0x00A;
  sf->rsi = (uint64_t)argc;
  sf->rdi = (uint64_t)process;
  sf->rbp = 0x00B;
  sf->rdx = (uint64_t)argv;
  sf->rcx = 0x00C;
  sf->rbx = 0x00D;
  sf->rax = 0x00E;
  sf->rip = (uint64_t)processWrapper;
  sf->cs = 0x008;
  sf->eflags = 0x202;
  sf->rsp = (uint64_t)(&sf->base);
  sf->ss = 0x000;
  sf->base = 0x000;
}

pcb* initializeBlock(char* name, priority_type foreground, int *fd) 
{
  if (foreground > 1) return NULL;
  pcb* newProcess = malloc(sizeof(pcb));

  if (newProcess == NULL) return NULL;
  newProcess->ppid = (currentProcessPCB != NULL) ? currentProcessPCB->pid : 0; 
  
  strcpy(newProcess->name, name);

  newProcess->pid = pidCounter++;
  newProcess->foreground = currentProcessPCB != NULL ? (currentProcessPCB->foreground && foreground) : 1; // chequear que el proceso actual sea foreground
  newProcess->state = READY;

  newProcess->priority = (foreground) ? FOREGROUND_PRIORITY : BACKGROUND_PRIORITY;
  void* tempRbp = malloc(sizeof(char) * STACK_SIZE);

  /* 
    Alocar file descriptors
  */

  if (tempRbp == NULL) {
    free(newProcess);
    return NULL;
  }

  newProcess->rbp = (void *)((char *)tempRbp + STACK_SIZE - 1);
  newProcess->rsp = (void *)((stackFrame *)newProcess->rbp - 1);

  return newProcess;
}

int changeState(int pid, process_state newState) {
  pcb *process = (pid == currentProcessPCB->pid) ? currentProcessPCB : getProcess(queue, pid);
  if (process == NULL || process->state == TERMINATED )
    return -1;
  
  if (process->pid != currentProcessPCB->pid) {
    // Cambia el contador de cuantos procesos listos hay para saber cuando activar el idle
    if (process->state != READY && newState == READY) {
      readyCount += 1;
    }

    if (process->state == READY && newState != READY) {
      readyCount -= 1;
    }
  } else if (newState == TERMINATED) {
    readyCount -= 1;
  }

  process->state = newState;
  return process->pid;
}

// Realizo una copia de los argumentos
int cpyArgs(char **dest, char **from, int count) {
  for (int i = 0; i < count; i += 1) {
    dest[i] = malloc(sizeof(char) * (strlen(from[i]) + 1));
    if (dest[i] == NULL) {
      i--;
      // Libero la memoria existente hasta ahora
      while(i >= 0) {
        free(dest[i]);
        i -= 1;
      }
      return -1;
    }
    strcpy(dest[i], from[i]);
  }
  return 0;
}