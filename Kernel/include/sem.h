#ifndef SEM_H
#define SEM_H

#include <stdint.h>

#define NULL (void *) 0

typedef struct t_pid_node {
  int pid;
  struct t_pid_node * next;
} t_pid_node;

typedef t_pid_node * t_pid_list;

typedef struct t_sem {
  t_pid_list blockedPidsList;
  uint16_t  attachedProcesses; //para init y destroy automatico
  uint32_t id;
  uint64_t value;
  struct t_sem *next;
  int lock;
} t_sem;

int semOpen(uint32_t id, uint64_t initialValue);
int semWait(uint32_t id);
int semPost(uint32_t id);
int semClose(uint32_t id);
void printSemInfo();

#endif