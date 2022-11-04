// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "usersyscalls.h"

char read()
{
  return _syscall(SYS_READ, 0, 0, 0, 0, 0);
}

int write(const char *buffer, uint64_t size, color_t *colors)
{
  return _syscall(SYS_WRITE, (uint64_t)buffer, (uint64_t)size, (uint64_t)colors, 0, 0);
}

int clear(uint8_t fd)
{
  return _syscall(SYS_CLEAN_SCREEN, fd, 0, 0, 0, 0);
}

int inforeg(uint64_t *buffer)
{
  return _syscall(SYS_INFOREG, 0, (uint64_t)buffer, 0, 0, 0);
}

int getRTC(uint64_t id)
{
  return _syscall(SYS_DATENTIME, (uint64_t)id, 0, 0, 0, 0);
}

int printMem(uint64_t direc, uint8_t *buffer, uint64_t bytes)
{
  return _syscall(SYS_PRINTMEM, (uint64_t)direc, (uint64_t)buffer, (uint64_t)bytes, 0, 0);
}

int sleep(uint64_t seconds)
{
  return _syscall(SYS_WAIT, (uint64_t)seconds, 0, 0, 0, 0);
}

// Muestra cursor, 0 para desactivar 1 para activar.
int showCursor(int active)
{
  return _syscall(SYS_SET_CURSOR, (uint64_t)active, 0, 0, 0, 0);
}

// Syscalls de manejo de memoria (heap)

void *malloc(size_t memory)
{
  return (void *)_syscall(SYS_MALLOC, memory, 0, 0, 0, 0);
}

void free(void *memory)
{
  _syscall(SYS_FREE, (uint64_t)memory, 0, 0, 0, 0);
}

memoryInfo *getMemoryInfo()
{
  return (memoryInfo *)_syscall(SYS_GETMEMINFO, 0, 0, 0, 0, 0);
}

// Syscalls de manejo de procesos

int getpid()
{
  return _syscall(SYS_GETPID, 0, 0, 0, 0, 0);
}

schedulerInfo *getSchedulerInfo()
{
  return (schedulerInfo *)_syscall(SYS_GETPROCESSESINFO, 0, 0, 0, 0, 0);
}

int kill(int pid)
{
  return _syscall(SYS_KILL, pid, 0, 0, 0, 0);
}

void nice(int pid, int newPriority)
{
  _syscall(SYS_NICE, pid, newPriority, 0, 0, 0);
}

int block(int pid)
{
  return _syscall(SYS_BLOCK, pid, 0, 0, 0, 0);
}

int resume(int pid)
{
  return _syscall(SYS_RESUME, pid, 0, 0, 0, 0);
}

void yield()
{
  _syscall(SYS_YIELD, 0, 0, 0, 0, 0);
}
int startTask(void (*process)(unsigned int argc, char **argv), int argc, char **argv, int foreground, int *fd)
{
  return _syscall(SYS_CREATE_PROCESS, (uint64_t)process, (uint64_t)argc, (uint64_t)argv, (uint64_t)foreground, (uint64_t)fd);
}

// Syscalls de manejo de semaforos

int semOpen(char *id, int initialValue)
{
  return _syscall(SYS_SEMOPEN, (uint64_t)id, (uint64_t)initialValue, 0, 0, 0);
}

int semInit(int initialValue)
{
  return _syscall(SYS_SEMINIT, initialValue, 0, 0, 0, 0);
}

int semWait(int id)
{
  return _syscall(SYS_SEMWAIT, id, 0, 0, 0, 0);
}

int semPost(int id)
{
  return _syscall(SYS_SEMPOST, id, 0, 0, 0, 0);
}
int semClose(int id)
{
  return _syscall(SYS_SEMCLOSE, id, 0, 0, 0, 0);
}
userlandSemInfo *getSemInfo()
{
  return (userlandSemInfo *)_syscall(SYS_SEMPRINT, 0, 0, 0, 0, 0);
}

int waitpid(int pid)
{
  return (int)_syscall(SYS_WAITPID, (uint64_t)pid, 0, 0, 0, 0);
}

// Syscalls de manejo de pipes

int pipeOpen(int pipeId)
{
  return _syscall(SYS_PIPEOPEN, pipeId, 0, 0, 0, 0);
}
int pipeRead(int pipeId)
{
  return _syscall(SYS_PIPEREAD, pipeId, 0, 0, 0, 0);
}
int pipeWrite(int pipeId, char *str)
{
  return _syscall(SYS_PIPEWRITE, (uint64_t)pipeId, (uint64_t)str, 0, 0, 0);
}
int pipeClose(int pipeId)
{
  return _syscall(SYS_PIPECLOSE, pipeId, 0, 0, 0, 0);
}
userlandPipeInfo *getPipeInfo()
{
  return (userlandPipeInfo *)_syscall(SYS_PIPEPRINT, 0, 0, 0, 0, 0);
}