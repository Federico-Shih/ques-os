#include "usersyscalls.h"
#include "ustdlib.h"
#include "phylo.h"
#include "test_util.h"

t_philosofer philosofers[MAX_PHILO] = {{0}};

static int currentPhilosofers = 0;

static int gateMutex;

void philoEat(int64_t index)
{
  philosofers[index].state = EATING;
  sys_wait(EATING);
}

void philoWait(int64_t index)
{
  philosofers[index].state = WAITING;
  sys_semWait(gateMutex);
  if (index % 2 == 0)
  {
    sys_semWait(philosofers[index].semId);
    sys_semWait(
      philosofers[
        (
          (index - 1) < 0 ? 
          (index - 1 + currentPhilosofers) : 
          index - 1
        )
      ].semId);
  } 
  else
  {
    sys_semWait(
      philosofers[
        (
          (index - 1) < 0 ? 
          (index - 1 + currentPhilosofers) : 
          index - 1
        )
      ].semId);
    sys_semWait(philosofers[index].semId);    
  }
}

void philoRest(int64_t index)
{
  sys_semPost(gateMutex);
  sys_semPost(philosofers[index].semId);
  sys_semPost(
    philosofers[
      (
        (index - 1) < 0 ? 
        (index - 1 + currentPhilosofers) : 
        index - 1
      )
    ].semId);
  philosofers[index].state = DECIDING;
  sys_wait(DECIDING);
}

void philoMain(unsigned int argc, char *argv[])
{
  int64_t index = satoi(argv[1]);
  while (1)
  {
    philoWait(index);
    philoEat(index);
    philoRest(index);
  }
}

int addPhilosofer(int index)
{
  if (currentPhilosofers < MAX_PHILO)
  {
    char *args[2] = { "philosofer" };
    args[1] = _itoa(index, 10);
    int processPid = sys_startTask(&philoMain, 2, args, 0, NULL);
    if (processPid == -1)
      return 0;
    philosofers[currentPhilosofers].pid = processPid;
    int semaphore = sys_semInit(1);
    if (semaphore == -1)
    {
      sys_kill(processPid);
      return 0;
    }
    philosofers[currentPhilosofers].semId = semaphore;
    currentPhilosofers += 1;
    return 1;
  }
  return 0;
}

int removePhilosofer()
{
  if (currentPhilosofers > 1)
  {
    sys_semWait(gateMutex); // Si tengo un filosofo menos, tengo que eliminar uno de los asientos
    sys_kill(philosofers[currentPhilosofers - 1].pid);
    currentPhilosofers -= 1;
    return 1;
  }
  return 0;
}

void printTable()
{
  while (1)
  {
    for (int i = 0; i < currentPhilosofers; i += 1)
    {
      if (philosofers[i].state == EATING)
        _fprintf( "%c ", 'E');
      else
        _fprintf( "%c ", '.');
    }
    _putc('\n');
    sys_wait(PRINT_TIME);
  }
}

void phylo()
{
  gateMutex = sys_semInit(4);
  if (gateMutex == -1)
    return;
  
  for (int i = 0; i < INITIAL_PHILO; i += 1)
  {
    addPhilosofer(i);
  }

  char *args[] = { "Print table" }; 
  sys_startTask(&printTable, 1, args, 0, NULL);
  while(1)
    sys_yield();
}