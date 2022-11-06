#ifndef PHYLO_H
#define PHYLO_H

typedef enum
{
  WAITING,
  DECIDING = 1,
  EATING = 2
} philo_state;

/*
  Representa a un filosofo en particular.
*/
typedef struct philosofer_t
{
  int pid;
  philo_state state;
  int semId;
} philosofer_t;

void phylo();

#define MAX_PHILO 15
#define PRINT_TIME 2
#define INITIAL_PHILO 5
#endif