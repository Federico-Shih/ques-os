#include "usersyscalls.h"
#include "ustdlib.h"
#include "phylo.h"
#include "test_util.h"

#define LEFT(i) (((i) + currentPhilosofers - 1) % currentPhilosofers)
#define RIGHT(i) (((i) + 1) % currentPhilosofers)

t_philosofer philosopher[MAX_PHILO] = {{0}};

static int currentPhilosofers = 0;

static int gateMutex;


void philoEat(int64_t index)
{
  philosopher[index].state = EATING;
  sys_wait(EATING);
}

static void testEat(int64_t index)
{
  if (philosopher[index].state == WAITING && philosopher[LEFT(index)].state != EATING &&  philosopher[RIGHT(index)].state != EATING)
  {
    philosopher[index].state = EATING;
    sys_semPost(philosopher[index].semId);
  }
}

void philoWait(int64_t index)
{
  sys_semWait(gateMutex);
  philosopher[index].state = WAITING;
  testEat(index);
  sys_semPost(gateMutex);
  sys_semWait(philosopher[index].semId);
}

void philoRest(int64_t index)
{
  sys_semWait(gateMutex);
  philosopher[index].state = DECIDING;
  testEat(LEFT(index));
  testEat(RIGHT(index));
  sys_semPost(gateMutex);
}

void philoMain(unsigned int argc, char *argv[])
{
  int64_t index = satoi(argv[1]);
  while (1)
  {
    philoWait(index);
    sys_wait(EATING);
    philoRest(index);
    sys_wait(DECIDING);
  }
}

int addPhilosofer()
{
  if (currentPhilosofers == MAX_PHILO)
  {
    return -1;
  }
  // Espero a que se libere el extremo necesario para que pueda meter otro filosofo
  sys_semWait(gateMutex);

  int semaphore = sys_semInit(1);
  philosopher[currentPhilosofers].state = DECIDING;

  if (semaphore == -1)
  {
    return -1;
  }
  philosopher[currentPhilosofers].semId = semaphore;

  char *args[2] = {"philosofer"};
  args[1] = _itoa(currentPhilosofers, 10);
  int processPid = sys_startTask(&philoMain, 2, args, 0, NULL);
  if (processPid == -1)
  {
    sys_semClose(philosopher[currentPhilosofers].semId);
    return -1;
  }
  philosopher[currentPhilosofers].pid = processPid;
  currentPhilosofers += 1;
  sys_semPost(gateMutex);
  return 0;
}

void freePhilosofer(int index)
{
  sys_kill(philosopher[index].pid);
  sys_semClose(philosopher[index].semId);
}

int removePhilosofer()
{
  if (currentPhilosofers == INITIAL_PHILO)
  {
    return -1;
  }

  sys_semWait(gateMutex);
  freePhilosofer(currentPhilosofers - 1);
  currentPhilosofers -= 1;
  sys_semPost(gateMutex);
  return 0;
}


void printTable()
{
  while (1)
  {
    sys_semWait(gateMutex);
    for (int i = 0; i < currentPhilosofers; i += 1)
    {
      if (philosopher[i].state == EATING)
        _fprintf("%c ", 'E');
      else
        _fprintf("%c ", '.');
    }
    _putc('\n');
    sys_semPost(gateMutex);
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

  char *args[] = {"Print table"};
  sys_startTask(&printTable, 1, args, 0, NULL);
  char c;
  while ((c = getChar()) != -1)
  {
    if (c == 'a')
    {
      addPhilosofer();
    }
    else if (c == 'd')
    {
      removePhilosofer();
    }
    else if (c == 'q')
    {
      break;
    }
  }

  for (int i = 0; i < currentPhilosofers; i += 1)
  {
    freePhilosofer(i);
  }
  sys_semClose(gateMutex);
}