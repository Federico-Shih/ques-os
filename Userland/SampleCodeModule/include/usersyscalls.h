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
  SYS_MALLOC,
  SYS_FREE,
  SYS_MEMDUMP,
} syscall_id;



extern uint64_t _syscall(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx);

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

#endif