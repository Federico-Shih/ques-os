#include <processManager.h>
#include <stdlib.h>
#include <interrupts.h>
#include <string.h>
#include <console.h>

#define STACK_SIZE (1024 * 4)

static int pidCounter = 1;
static queueADT queue = NULL;
static int currentCycles;

// Datos de proceso inactivo
static pcb *idleProcessPCB = NULL;

// Datos del proceso actual
static pcb *currentProcessPCB = NULL;

// Cantidad de procesos listos
static int readyCount = 0;

// Proceso inactivo
void idleProcess(int argc, char **argv)
{
  while (1)
    _hlt();
}

// Comenzar scheduler
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
  idleProcessPCB = (pcb *)dequeue(queue);
}

// Libera la memoria de un proceso particular
void freeProcess(pcb* process) {
  for (int i = 0; i < process->argc; i += 1) {
    free(process->argv[i]);
  }
  free(process->argv);
  // Previamente se hizo malloc en el stack del proceso
  free((void *)((char *)process->rbp - STACK_SIZE + 1));
  free((void *)process);
}

// Programa una tarea a realizar
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
        enqueue(queue, (void *)currentProcessPCB);
      }
    } else {
      idleProcessPCB = currentProcessPCB;
    }
  }

  if (readyCount > 0) 
  {
    currentProcessPCB = (pcb *)dequeue(queue);

    // Como readyCount > 0, me aseguro que existe algun proceso
    while (currentProcessPCB->state != READY) {
      if (currentProcessPCB->state == TERMINATED) 
      {
        freeProcess(currentProcessPCB);
      }
      if (currentProcessPCB->state == BLOCKED)
      {
        enqueue(queue, (void *)currentProcessPCB);
      }
      currentProcessPCB = dequeue(queue);
    }  
  } 
  else 
  {
    currentProcessPCB = idleProcessPCB;
  }
  
  currentCycles = currentProcessPCB->priority;
  return currentProcessPCB->rsp;
}

// Comienza una tarea
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
  
  enqueue(queue, (void *)newProcess);
  readyCount += 1;
  // Bloqueo el padre.
  if (newProcess->foreground && newProcess->ppid) {
    blockTask(newProcess->ppid);
  }

  return newProcess->pid;
} 

// Imprime el estado y datos de todas las tareas actuales
int printTasks()
{
  pcb *aux;
  toBegin(queue);
  while(hasNext(queue)){
    aux = next(queue);
    printTask(aux->pid);
  }
  return 0;
}

// Imprime el estado y datos de una tarea particular
int printTask(int pid)
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
  return 0;
}

// Funcion que retorna un string diferenciando si un proceso es foreground o no
char *foregToBool(int foreground)
{
  return foreground ? "TRUE" : "FALSE";
}

// Funcion que retorna un string del estado de un proceso
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

// retorna el id del proceso actual
int getpid()
{
  return (currentProcessPCB != NULL) ? currentProcessPCB->pid : 0;
}

// Termina el proceso especificado
int killTask(int pid)
{
  int id = changeState(pid, TERMINATED);

  // No se encontro el proceso
  if( id == -1 )
    return -1; 

  if(id == currentProcessPCB->pid)
    _callTimerTick();

  return id;
}

// Cambia la prioridad del proceso especificado
int nice(int pid, int newPriority)
{
  if( newPriority < 0)
    newPriority = 0;
  if( newPriority > MAX_PRIO)
    newPriority = MAX_PRIO;

  pcb *process = getProcess(queue, pid);   

  if(process == NULL) {
    return -1;
  } 
  process->priority = newPriority; 
  return 0;
}

// Bloquea el proceso especificado
int blockTask(int pid)
{
  int id = changeState(pid, BLOCKED);
  if( id == -1 )
    return -1;
  if( id == currentProcessPCB->pid)
    _callTimerTick();
  return 0;
}

// Desbloque el proceso especificado
int resumeTask(int pid)
{
  int id = changeState(pid, READY);
  if(id == -1 )
    return -1;
  return 0;
}

// Renuncia al CPU
int yield()
{
  currentCycles = 0;
  _callTimerTick();
  return 0;
}

// Termina el proceso actual
int killCurrent() {
  return killTask(currentProcessPCB->pid);
}


static void processWrapper(void (*process)(int, char**), int argc, char **argv) 
{
  process(argc, argv);
  killCurrent();
}

// Inicializa el stack
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

// Inicializa el bloque de memoria
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

// Cambia el estado del proceso especificado
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

// Devuelve los datos del proceso especificado
pcb *getProcess(queueADT queue, int pid)
{
  toBegin(queue);
  while(hasNext(queue))
  {
    pcb *process =(pcb *) next(queue);
    if(pid == process->pid)
      return next;
  }
  return NULL;
}
