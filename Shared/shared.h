#ifndef SHARED_H
#define SHARED_H

#include <stdint.h>

// ------------ SEMAFOROS -----------------------------
typedef struct userlandBlockedPids {
  int * array; 
  int length;
} userlandBlockedPids;

typedef struct userlandSem {
  uint16_t attachedProcesses;
  int id;
  int value;
  int lock;
  char *name;
  userlandBlockedPids * blockedPids;
} userlandSem;

typedef struct userlandSemInfo {
    userlandSem * array;
    int length;
} userlandSemInfo;

// ------------ PIPES ----------------------------
typedef struct userlandPipe {
  int id;
  int writeIndex, readIndex;
  int totalProcesses;
  userlandSem * readSem;
  userlandSem * writeSem;
} userlandPipe;

typedef struct userlandPipeInfo {
  userlandPipe * array;
  int length;
  int pipeBufferSize;
} userlandPipeInfo;

// ------------ SCHEDULER ----------------------------

typedef struct processInfo {
  char name[30];
  int pid;
  int ppid;
  int foreground;
  uint64_t rsp;
  uint64_t rbp;
  char state[20];
  int priority;
} processInfo;

typedef struct schedulerInfo {
  processInfo * array;
  int length;
} schedulerInfo;

//------------- MEMORY --------------------
typedef struct memoryInfo {
  uint64_t availableMem;
  uint64_t totalMem;
} memoryInfo;

#endif