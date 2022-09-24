#ifndef SHELL_H
#define SHELL_H
#include <commands.h>

typedef enum {
  RUNNING, PAUSED, ENDED
} STATUS;

typedef enum {
  ESCAPE=0x01, P=0x19, A=0x1E, D=0x20
} SCANCODE;

void runShell();
void initShell();


typedef struct caller {
  int (*runner)(unsigned int count, void** args);
  void** args;
  unsigned int argCount;
  unsigned int screenId;
} caller;

#endif