#ifndef PIPES_H
#define PIPES_H

#define PIPE_BUFFER_SIZE 1024
#define STDIN_PIPENO 0

#include <stdint.h>

int initPipeSystem();
int pipeOpen(int pipeId);
int pipeWrite(int pipeId, char *str);
int pipeClose(int pipeId);
int pipeRead(int pipeId);
void printPipeInfo();
int pipePutchar(int pipeId, char c);

#endif