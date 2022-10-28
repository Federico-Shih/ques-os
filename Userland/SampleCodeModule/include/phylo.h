#ifndef PHYLO_H
#define PHYLO_H

typedef enum { WAITING, DECIDING=1,  EATING=2 } philo_state;

/*
  Representa a un filosofo en particular
  semId representa el tenedor a su derecha. Para comer, tendra que agarrar
  el tenedor a su derecha philosofers[index].semId y el de la izquierda 
  philosofers[(index - 1) % currentPhilosofers].semId.
*/
typedef struct t_philosofer
{
  int pid;
  philo_state state;
  int semId;
} t_philosofer;

void phylo();

#define MAX_PHILO 15
#define PRINT_TIME 2
#define INITIAL_PHILO 5
#endif