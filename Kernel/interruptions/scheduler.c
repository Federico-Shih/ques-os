#include <scheduler.h>
#include <stdint.h>
#include <interrupts.h>
#include <console.h>

const static uint64_t baseRSP = 0x200000;

static uint64_t sampleCodeModuleRSP;

static uint64_t STARTPID = 1000;
#define TASKQUANTITY 16
#define PAGESIZE 1024

static task taskSchedule[TASKQUANTITY] = {{{0}}};
static int currentTask = 0;
static int tasks = 0;
int findNextFreeTask();
int findTask(uint64_t pid);

// Devuelve la proxima task que puede ser corrida, los no inicializados y los que estan corriendo
int nextRunnableTask() {
  // Si vuelve a la misma task, que termine
  for (int i = 1; i < TASKQUANTITY; i += 1) {
    int index = (i + currentTask) % TASKQUANTITY;
    if (taskSchedule[index].status == RUNNING || taskSchedule[index].status == NOTINITIALIZED) {
      return index;
    }
  }
  return currentTask;
}

// Solo devuelve aquellos que ya estan inicializados, sirve para casos en el cual queres forzar el cambio
int nextActiveTask() {
  for (int i = 1; i < TASKQUANTITY; i += 1) {
    int index = (i + currentTask) % TASKQUANTITY;
    if (taskSchedule[index].status == RUNNING) {
      return index;
    }
  }
  return currentTask;
}

uint64_t switchTask(uint64_t rsp, bool forced) {
  // Si no hay tasks desde el cual cambiar, que continue normalmente
  if (tasks > 1) {
    // Guarda el rsp actual
    if (!forced) {
      taskSchedule[currentTask].rsp = rsp;
      currentTask = nextRunnableTask();
    } else {
      // No correr nuevo programa
      currentTask = nextActiveTask();
    }
    task* curr = &(taskSchedule[currentTask]);
    switchScreens(curr->program.screenId);
    if (curr->status == NOTINITIALIZED) {
      curr->status = RUNNING;
      switchRsp(currentTask * PAGESIZE + baseRSP);
      endInterrupt();
      curr->program.runner(curr->program.argCount, curr->program.args);    
      switchTask(0, true);
    }
    endInterrupt();
    switchContext(curr->rsp);
  }
  return rsp;
}

uint64_t getPid() {
  if (tasks > 0) {
    return taskSchedule[currentTask].id;
  }
  return 0;
}

// Devuelve el pid de la task creada, sino devuelve overflow
uint64_t loadTask(caller* function, int position, uint64_t parentId) {
  if (tasks == TASKQUANTITY) {
    return 0;
  }
  taskSchedule[position].status = NOTINITIALIZED;
  taskSchedule[position].id = STARTPID++;
  taskSchedule[position].program = *function;
  taskSchedule[position].parentId = parentId;
  taskSchedule[position].rsp = 0;
  tasks += 1;
  return taskSchedule[position].id;
}

uint64_t startChild(caller* function) {
  int freeIndex = findNextFreeTask();
  return loadTask(function, freeIndex, (tasks == 0) ? 0 : taskSchedule[currentTask].id);
}

// Comienza una task y para la ejecucion del programa que lo llamo
void startTask(caller* function, uint64_t rsp) {
  if (tasks > 0) {
    taskSchedule[currentTask].status = PAUSED;
    taskSchedule[currentTask].rsp = rsp;
  } else {
    // Si no hay tasks y se hizo startTask, significa que se llamo desde el sampleCodeModule
    sampleCodeModuleRSP = rsp;
  }
  int freeIndex = findNextFreeTask();
  uint64_t started = loadTask(function, freeIndex, (tasks == 0) ? 0 : taskSchedule[currentTask].id);
  if (!started) return;
  
  taskSchedule[freeIndex].status = RUNNING;

  // Cambia la task a correr
  currentTask = freeIndex;

  switchScreens(function->screenId);

  caller* program = &(taskSchedule[freeIndex].program);
  switchRsp(freeIndex * PAGESIZE + baseRSP);
  endInterrupt();
  program->runner(program->argCount, program->args);
  switchTask(0, true);
}

bool hasChilds(uint64_t pid) {
  for (int i = 0; i < TASKQUANTITY; i++) {
    if (taskSchedule[i].parentId == pid && taskSchedule[i].status != NOTPRESENT) {
      return true;
    }
  }
  return false;
}

bool currentHasChilds() {
  for (int i = 0; i < TASKQUANTITY; i += 1) {
    if (taskSchedule[i].parentId == taskSchedule[currentTask].id && taskSchedule[i].status != NOTPRESENT) {
      return true;
    }
  }
  return false;
}


int findNextFreeTask() {
  for (int i = 0; i < TASKQUANTITY; i++) {
    if (taskSchedule[i].status == NOTPRESENT) return i;
  }
  return -1;
}

// Mata recursivamente una task. Toda task terminada tiene que terminar sus hijos
// Devuelve verdadero si fue terminado, falso si no
// Se fija que la task padre si tiene hijos o no. Si no tiene hijos, resumirlo.
bool killTask(uint64_t pid) {
  int index = findTask(pid);
  if (index == -1 || taskSchedule[index].status == NOTPRESENT) return false;
  for (int i = 0; i < TASKQUANTITY; i++) {
    if (taskSchedule[i].parentId == pid) {
      killTask(taskSchedule[i].id);
    }
  }
  tasks -= 1;
  taskSchedule[index].status = NOTPRESENT;

  if (taskSchedule[index].parentId == 0) {
    // Si se mata la funcion que llamo el sampleCodeModule (ej: shell), necesito que vuelva al sampleCodeModule
    switchContext(sampleCodeModuleRSP);
    return true;
  }
  int parentIndex = findTask(taskSchedule[index].parentId);
  int killedIndex = currentTask;
  if (!hasChilds(taskSchedule[index].parentId)) {
    taskSchedule[parentIndex].status = RUNNING;
    currentTask = parentIndex;
  }

  if (pid == taskSchedule[killedIndex].id) {
    // Cambiar la task a la siguiente que se puede correr. No queremos que vuelva
    switchTask(0, true); 
  }
  return true;
}

int findTask(uint64_t pid){
    for(int i = 0; i < TASKQUANTITY; i++){
        if(taskSchedule[i].id == pid && taskSchedule[i].status != NOTPRESENT)
            return i;
    }
    return -1;
}

bool pauseTask(uint64_t pid){
  int index = findTask(pid);
  if (index == -1){
    return false;
  }
  taskSchedule[index].status = PAUSED;
  return true; 
}

int resumeTask(uint64_t pid ){
  int index = findTask(pid);
  if(index == -1){
    return false;
  }
  taskSchedule[index].status = RUNNING;
  return true;
}