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
  queueADT processQueue = getProcessQueue();
  while (1)
  {
    // Hubo una muerte de proceso
    toBegin(processQueue);

    // Los marca como para ser matados
    while (hasNext(processQueue))
    {
      pcb *process = next(processQueue);
      if (process->ppid == getpid() && process->state == EXITED)
      {
        process->state = TERMINATED;
      }
    }

    sys_wait(FLUSH_TIME);
  }
}
