// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ustdlib.h"
#include "test_util.h"
#include "usersyscalls.h"

enum State
{
  RUNNING,
  BLOCKED,
  KILLED
};

typedef struct P_rq
{
  int32_t pid;
  enum State state;
} p_rq;

void test_processes(unsigned int argc, char *argv[])
{
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {"endless_loop"};

  if (argc < 2)
    return;

  if ((max_processes = satoi(argv[1])) <= 0)
    return;
  p_rq p_rqs[max_processes];

  while (1)
  {
    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++)
    {
      p_rqs[rq].pid = sys_startTask(&endless_loop, 1, argvAux, 0, NULL);

      if (p_rqs[rq].pid == -1)
      {
        _fprintf("test_processes: ERROR creating process\n");
        return;
      }
      else
      {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }
    sys_wait(1);
    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0)
    {
      for (rq = 0; rq < max_processes; rq++)
      {
        action = GetUniform(100) % 2;

        switch (action)
        {
        case 0:
          if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED)
          {
            if (sys_kill(p_rqs[rq].pid) == -1)
            {
              _fprintf("test_processes: ERROR killing process\n");
              return;
            }
            p_rqs[rq].state = KILLED;
            alive--;
          }
          break;

        case 1:
          if (p_rqs[rq].state == RUNNING)
          {
            if (sys_block(p_rqs[rq].pid) == -1)
            {
              _fprintf("test_processes: ERROR blocking process\n");
              return;
            }
            p_rqs[rq].state = BLOCKED;
          }
          break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
      {
         if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2)
        {
          if (sys_resume(p_rqs[rq].pid) == -1)
          {
            _fprintf("test_processes: ERROR unblocking process\n");
            return;
          }
          p_rqs[rq].state = RUNNING;
        }
      }
    }

    for (rq = 0; rq < max_processes; rq++)
    {
      waitpid(p_rqs[rq].pid);
    }
    sys_wait(1);
  }
}