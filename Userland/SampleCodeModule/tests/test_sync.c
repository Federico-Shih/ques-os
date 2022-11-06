// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "test_util.h"
#include "usersyscalls.h"
#include "ustdlib.h"

#define SEM_ID "test_sync_sem"
#define TOTAL_PAIR_PROCESSES 4

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc)
{
  uint64_t aux = *p;
  yield(); // This makes the race condition highly probable
  _fprintf(".");
  aux += inc;
  *p = aux;
}

void my_process_inc(unsigned int argc, char *argv[])
{
  int64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 4)
    return;

  if ((n = satoi(argv[1])) <= 0)
    return;
  if ((inc = satoi(argv[2])) == 0)
    return;
  if ((use_sem = satoi(argv[3])) < 0)
    return;

  int sharedSem;
  if (use_sem)
    if ((sharedSem = semOpen(SEM_ID, 1)) == -1)
    {
      _fprintf("test_sync: ERROR opening semaphore\n");
      return;
    }

  uint64_t i;
  for (i = 0; i < n; i++)
  {
    if (use_sem)
      semWait(sharedSem);
    slowInc(&global, inc);
    if (use_sem)
      semPost(sharedSem);
  }

  if (use_sem)
    semClose(sharedSem);

  return;
}

// Los parametros a recibir son arg[1] = # de inc, arg[2] = 0 o 1 si se usa semaforo o no
void test_sync(unsigned int argc, char *argv[])
{ //{n, use_sem, 0}
  int pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 3)
  {
    _fprintf("# de args equivocados.");
    return;
  }
  if ((satoi(argv[1])) <= 0)
  {
    _fprintf("arg0 no es > 0");
    return;
  }

  char *argvDec[] = {"dec_process", argv[1], "-1", argv[2]};
  char *argvInc[] = {"inc_process", argv[1], "1", argv[2]};

  global = 0;

  int i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    pids[i] = startTask(&my_process_inc, 4, argvDec, 0, NULL);
    pids[i + TOTAL_PAIR_PROCESSES] = startTask(&my_process_inc, 4, argvInc, 0, NULL);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    waitpid(pids[i]);
    waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  _fprintf("Final value: %d\n", global);

  return;
}