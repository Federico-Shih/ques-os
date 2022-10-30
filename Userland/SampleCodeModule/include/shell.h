#ifndef SHELL_H
#define SHELL_H
#include <commands.h>

typedef enum {
  ESCAPE=0x01, P=0x19, A=0x1E, D=0x20
} SCANCODE;

void runShell();
void initShell();
int findPipe(int argc, char **argv);
int linkPipe(int argc, char **argv, int pipeIndex);
int runPipeCommand(int argc, char **argv, int fdin, int fdout);


typedef struct caller {
  void (*runner)(unsigned int count, char** args);
  char** args;
  unsigned int argCount;
  unsigned int screenId;
} caller;

#endif