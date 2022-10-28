#include "processManager.h"
#include "sem.h"
#include "queue.h"
#include "initProcess.h"
#include "syscalls.h"

#define FLUSH_TIME 10

queueADT initProcessQueue = NULL;

// Proceso que mata regularmente procesos hijo que no fueron liberados por wait
void init()
{
  int initSem = semOpen(INIT_SEM, 0);
  queueADT processQueue = getProcessQueue();
  while (1)
  {
    // Hubo una muerte de proceso
    semWait(initSem);
    toBegin(processQueue);

    // Los marca como para ser matados
    while (hasNext(processQueue))
    {
      pcb *process = next(processQueue);
      if (process->ppid == getpid() && semValue(process->semId) > 0 && process->state == EXITED)
      {
        process->state = TERMINATED;
      }
    }

    sys_wait(FLUSH_TIME);
  }
}
