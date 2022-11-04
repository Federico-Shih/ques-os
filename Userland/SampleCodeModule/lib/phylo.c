// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "usersyscalls.h"
#include "ustdlib.h"
#include "phylo.h"
#include "test_util.h"

#define LEFT(i) (((i) + currentPhilosofers - 1) % currentPhilosofers)
#define RIGHT(i) (((i) + 1) % currentPhilosofers)

t_philosofer philosopher[MAX_PHILO] = {{0}};

static int currentPhilosofers = 0;

static int gateMutex;

static void testEat(int64_t index)
{
  if (philosopher[index].state == WAITING && philosopher[LEFT(index)].state != EATING && philosopher[RIGHT(index)].state != EATING)
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

int startPhiloProcess()
{
  philosopher[currentPhilosofers].state = DECIDING;
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
  return 0;
}

int addPhilosofer()
{
  if (currentPhilosofers == MAX_PHILO)
  {
    return -1;
  }

  int semaphore = sys_semInit(1);
  if (semaphore == -1)
  {
    return -1;
  }
  philosopher[currentPhilosofers].semId = semaphore;

  sys_semWait(gateMutex);
  startPhiloProcess();
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
static color_t standard[] = {WHITE, BLACK};
static color_t phyloAdd[] = {BLUE, BLACK};
static color_t phyloRemove[] = {BROWN, BLACK};
static color_t phyloQuit[] = {GREEN, BLACK};
static color_t errorPhylo[] = {BLACK, RED};
void phylo()
{
  gateMutex = sys_semInit(1);
  if (gateMutex == -1)
    return;

  currentPhilosofers = 0;

  sys_write("Pulse la tecla", 14, standard);
  sys_write(" a ", 3, phyloAdd);
  sys_write("para agregar un filosofo comensal. \n", 36, standard);
  sys_write("Pulse la tecla", 14, standard);
  sys_write(" d ", 3, phyloRemove);
  sys_write("para eliminar un filosofo comensal. \n", 37, standard);
  sys_write("Pulse la tecla", 14, standard);
  sys_write(" q ", 3, phyloQuit);
  sys_write("para cerrar la mesa. \n", 23, standard);

  sys_semWait(gateMutex);
  for (int i = 0; i < INITIAL_PHILO; i += 1)
  {
    int semaphore = sys_semInit(1);
    if (semaphore == -1)
    {
      _fprintf("ERROR: No se pudo inicializar semaforo.\n");
      return;
    }
    philosopher[i].semId = semaphore;
  }
  for (int i = 0; i < INITIAL_PHILO; i += 1)
  {
    if (startPhiloProcess() == -1)
    {
      _fprintf("ERROR: No se pudo inicializar proceso de filosofo.\n");
      return;
    }
  }

  char *args[] = {"Print table"};
  int tablePid = sys_startTask(&printTable, 1, args, 0, NULL);
  if (tablePid == -1)
  {
    _fprintf("ERROR: No se pudo inicializar proceso de mesa.");
    return;
  }
  sys_semPost(gateMutex);
  char c;
  while ((c = getChar()) != -1)
  {
    if (c == 'a')
    {
      int addedPhilo = addPhilosofer();
      if (addedPhilo == 0)
        sys_write("Un filosofo ha llegado a la mesa para disfrutar del queso. \n", 61, phyloAdd);
      else
        sys_write("No hay mas espacio en la mesa\n", 32, errorPhylo);
    }
    else if (c == 'd')
    {
      int removedPhilo = removePhilosofer();
      if (removedPhilo == 0)
        sys_write("Un filosofo ha concluido su tiempo en la mesa del queso. \n", 59, phyloRemove);
      else
        sys_write("Minima cantidad de filosofos alcanzado\n", 40, errorPhylo);
    }
    else if (c == 'q')
    {
      sys_write("Y en ese momento\nuna gran ola llego,\nretirando a los filosofos\nhacia un lugar mejor. \n", 87, phyloQuit);
      break;
    }
  }
  if (c == -1)
    _fprintf("EOF\n");
  for (int i = 0; i < currentPhilosofers; i += 1)
  {
    freePhilosofer(i);
  }
  sys_kill(tablePid);
  sys_semClose(gateMutex);
}