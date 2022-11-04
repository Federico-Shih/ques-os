#ifndef SHARED_H
#define SHARED_H

#include <stdint.h>

// ------------ SEMAFOROS -----------------------------
typedef struct userlandBlockedPids {
  int * array; //free
  int length;
} userlandBlockedPids;

typedef struct userlandSem {
  uint16_t attachedProcesses;
  int id;
  int value;
  int lock;
  char *name; //free
  userlandBlockedPids * blockedPids; //free
} userlandSem;

typedef struct userlandSemInfo {
    userlandSem * array; //free
    int length;
} userlandSemInfo;

// ------------ PIPES ----------------------------
typedef struct userlandPipe {
  int id;
  int writeIndex, readIndex;
  int totalProcesses;
  userlandSem * readSem; //free
  userlandSem * writeSem; //free
} userlandPipe;

typedef struct userlandPipeInfo {
  userlandPipe * array; //free
  int length;
  int pipeBufferSize;
} userlandPipeInfo;

// ------------ SCHEDULER ----------------------------

/*

      printf(
        "Nombre: %s | PID: %d | PPID: %d | Foreground: %s | RSP: %x | RBP: "
        "%x | Prioridad: %d | Estado: %s \n",
        process->name, process->pid, process->ppid,
        foregToBool((int)process->foreground), (uint64_t)process->rsp,
        (uint64_t)process->rbp, process->priority,
        stateToStr(process->state));
*/

typedef struct processInfo {
  char name[30];
  int pid;
  int ppid;
  int foreground;
  uint64_t rsp;
  uint64_t rbp;
  char state[20];
} processInfo;

typedef struct schedulerInfo {
  processInfo * array;
  int length;
} schedulerInfo;

#endif