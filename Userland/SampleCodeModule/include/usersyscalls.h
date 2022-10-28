#ifndef USERSYSCALLS_H
#define USERSYSCALLS_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "shell.h"

typedef enum {
  SYS_READ=0, 
  SYS_WRITE, 
  SYS_CLEAN_SCREEN, 
  SYS_INFOREG, 
  SYS_DATENTIME, 
  SYS_PRINTMEM, 
  SYS_SET_CURSOR, 
  SYS_WAIT,
  SYS_GETKEY,
  // Memory Manager
  SYS_MALLOC,
  SYS_FREE,
  SYS_MEMDUMP,
  // Scheduler
  SYS_CREATE_PROCESS,
  SYS_GETPID,
  SYS_PRINTPROCESSES,
  SYS_PRINTPROCESS,
  SYS_KILL,
  SYS_EXIT,
  SYS_NICE,
  SYS_BLOCK,
  SYS_RESUME,
  SYS_YIELD,
  SYS_WAITPID,
  // Semaphores
  SYS_SEMOPEN,
  SYS_SEMINIT,
  SYS_SEMWAIT,
  SYS_SEMPOST,
  SYS_SEMCLOSE,
  SYS_SEMPRINT,
  // Pipes
  SYS_PIPEOPEN,
  SYS_PIPECLOSE,
  SYS_PIPEREAD,
  SYS_PIPEWRITE,
  SYS_PIPEPRINT
} syscall_id;




extern uint64_t _syscall(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

int sys_read(uint8_t fd, char* buffer, size_t count);
int sys_write(uint8_t fd, const char* buffer, size_t count);
int sys_clear(uint8_t fd);
int sys_inforeg(uint64_t* buffer);
int sys_dateAndTime(uint64_t id);
int sys_showCursor(int active);
int sys_printMem(uint64_t direc, uint8_t* buffer, uint64_t bytes);
int sys_wait(uint64_t seconds);
int sys_getKey(uint8_t fd, int* buffer, size_t count);
void* sys_malloc(size_t memory);
void sys_free(void* memory);
void sys_memDump();
int sys_getpid();
void sys_printProcesses();
void sys_printProcess(int pid);
int sys_kill(int pid);
void sys_nice(int pid, int newPriority);
int sys_block(int pid);
int sys_resume(int pid);
void sys_yield();
int sys_startTask(void (*process)(unsigned int argc, char **argv), int argc, char **argv, int foreground);
int sys_semOpen(char *semaphoreName, int initialValue);
int sys_semInit(int initialValue);
int sys_semWait(int id);
int sys_semPost(int id);
int sys_semClose(int id);
void sys_printSemInfo();
int sys_pipeOpen(int pipeId);
int sys_pipeWrite(int pipeId, char *str);
int sys_pipeClose(int pipeId);
int sys_pipeRead(int pipeId);
void sys_printPipeInfo();
int waitpid(int pid);

#endif