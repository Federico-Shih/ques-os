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
    semPost(philosopher[index].semId);
  }
}

void philoWait(int64_t index)
{
  semWait(gateMutex);
  philosopher[index].state = WAITING;
  testEat(index);
  semPost(gateMutex);
  semWait(philosopher[index].semId);
}

void philoRest(int64_t index)
{
  semWait(gateMutex);
  philosopher[index].state = DECIDING;
  testEat(LEFT(index));
  testEat(RIGHT(index));
  semPost(gateMutex);
}

void philoMain(unsigned int argc, char *argv[])
{
  int64_t index = satoi(argv[1]);
  while (1)
  {
    philoWait(index);
    sleep(EATING);
    philoRest(index);
    sleep(DECIDING);
  }
}

int startPhiloProcess()
{
  philosopher[currentPhilosofers].state = DECIDING;
  char *args[2] = {"philosofer"};
  args[1] = _itoa(currentPhilosofers, 10);
  int processPid = startTask(&philoMain, 2, args, 0, NULL);
  if (processPid == -1)
  {
    semClose(philosopher[currentPhilosofers].semId);
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

  int semaphore = semInit(1);
  if (semaphore == -1)
  {
    return -1;
  }
  philosopher[currentPhilosofers].semId = semaphore;

  semWait(gateMutex);
  startPhiloProcess();
  semPost(gateMutex);
  return 0;
}

void freePhilosofer(int index)
{
  kill(philosopher[index].pid);
  semClose(philosopher[index].semId);
}

int removePhilosofer()
{
  if (currentPhilosofers == INITIAL_PHILO)
  {
    return -1;
  }

  semWait(gateMutex);
  freePhilosofer(currentPhilosofers - 1);
  currentPhilosofers -= 1;
  semPost(gateMutex);
  return 0;
}

void printTable()
{
  while (1)
  {
    semWait(gateMutex);
    for (int i = 0; i < currentPhilosofers; i += 1)
    {
      if (philosopher[i].state == EATING)
        _fprintf("%c ", 'E');
      else
        _fprintf("%c ", '.');
    }
    _putc('\n');
    semPost(gateMutex);
    sleep(PRINT_TIME);
  }
}
static color_t standard[] = {WHITE, BLACK};
static color_t phyloAdd[] = {BLUE, BLACK};
static color_t phyloRemove[] = {BROWN, BLACK};
static color_t phyloQuit[] = {GREEN, BLACK};
static color_t errorPhylo[] = {BLACK, RED};
void phylo()
{
  gateMutex = semInit(1);
  if (gateMutex == -1)
    return;

  currentPhilosofers = 0;

  write("Pulse la tecla", 14, standard);
  write(" a ", 3, phyloAdd);
  write("para agregar un filosofo comensal. \n", 36, standard);
  write("Pulse la tecla", 14, standard);
  write(" d ", 3, phyloRemove);
  write("para eliminar un filosofo comensal. \n", 37, standard);
  write("Pulse la tecla", 14, standard);
  write(" q ", 3, phyloQuit);
  write("para cerrar la mesa. \n", 23, standard);

  semWait(gateMutex);
  for (int i = 0; i < INITIAL_PHILO; i += 1)
  {
    int semaphore = semInit(1);
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
  int tablePid = startTask(&printTable, 1, args, 0, NULL);
  if (tablePid == -1)
  {
    _fprintf("ERROR: No se pudo inicializar proceso de mesa.");
    return;
  }
  semPost(gateMutex);
  char c;
  while ((c = getChar()) != -1)
  {
    if (c == 'a')
    {
      int addedPhilo = addPhilosofer();
      if (addedPhilo == 0)
        write("Un filosofo ha llegado a la mesa para disfrutar del queso. \n", 61, phyloAdd);
      else
        write("No hay mas espacio en la mesa\n", 32, errorPhylo);
    }
    else if (c == 'd')
    {
      int removedPhilo = removePhilosofer();
      if (removedPhilo == 0)
        write("Un filosofo ha concluido su tiempo en la mesa del queso. \n", 59, phyloRemove);
      else
        write("Minima cantidad de filosofos alcanzado\n", 40, errorPhylo);
    }
    else if (c == 'q')
    {
      write("Y en ese momento\nuna gran ola llego,\nretirando a los filosofos\nhacia un lugar mejor. \n", 87, phyloQuit);
      break;
    }
  }
  for (int i = 0; i < currentPhilosofers; i += 1)
  {
    freePhilosofer(i);
  }
  kill(tablePid);
  semClose(gateMutex);
}