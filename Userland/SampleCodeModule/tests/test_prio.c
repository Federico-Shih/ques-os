
#include <stdint.h>
#include "ustdlib.h"
#include "test_util.h"

#define TOTAL_PROCESSES 3
#define LOWEST 1 //TODO: Change as required
#define MEDIUM 10 //TODO: Change as required
#define HIGHEST 20 //TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void printWrapper()
{
  endless_loop_print(MINOR_WAIT);
}

void test_prio(){
  int pids[TOTAL_PROCESSES]; 
  char *argv[] = {0};
  uint64_t i;

  for(i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = sys_startTask(printWrapper, 0, argv, 0);
  bussy_wait(WAIT);
  _fprintf(0, "\nCHANGING PRIORITIES...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_nice(pids[i], prio[i]);

  bussy_wait(WAIT);
  _fprintf(0, "\nBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_block(pids[i]);

  _fprintf(0, "CHANGING PRIORITIES WHILE BLOCKED...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_nice(pids[i], MEDIUM);

  _fprintf(0, "UNBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_resume(pids[i]);

  bussy_wait(WAIT);
  _fprintf(0, "\nKILLING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    sys_kill(pids[i]);
}