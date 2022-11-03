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

#endif