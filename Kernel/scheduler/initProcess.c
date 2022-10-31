// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
    iteratorADT it = toBegin(processQueue);

    // Los marca como para ser matados
    while (hasNext(it))
    {
      pcb *process = (pcb *)next(it);
      if (process->ppid == getpid() && process->state == EXITED)
      {
        process->state = TERMINATED;
      }
    }
    free(it);
    sys_wait(FLUSH_TIME);
  }
}
