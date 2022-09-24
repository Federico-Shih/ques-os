#ifndef USERSYSCALLS_H
#define USERSYSCALLS_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "shell.h"

enum {
  SYS_READ_ID, 
  SYS_WRITE_ID,
  SYS_CLEAR_ID, 
  SYS_INFOREG_ID, 
  SYS_DNT_ID,
  SYS_PRINTMEM_ID,
  SYS_CURSOR_ID, 
  SYS_SWITCHSCREEN_ID, 
  SYS_TOGGLEMODE_ID,
  SYS_START_ID,
  SYS_CHILD_ID,
  SYS_EXIT_ID,
  SYS_KILL_ID,
  SYS_PAUSE_ID,
  SYS_RESUME_ID,
  SYS_GETPID_ID,
  SYS_HASCHILD,
  SYS_WAIT_ID,
  SYS_GETKEY_ID
};



extern uint64_t _syscall(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx);

int sys_read(uint8_t fd, char* buffer, size_t count);
int sys_write(uint8_t fd, const char* buffer, size_t count);
int sys_clear(uint8_t fd);
int sys_inforeg(uint64_t* buffer);
int sys_dateAndTime(uint64_t id);
int sys_showCursor(int active);
int sys_switchScreen(size_t screen);
int sys_toggleMode(int mode);
int sys_printMem(uint64_t direc, uint8_t* buffer, uint64_t bytes);
int sys_wait(uint64_t seconds);
uint64_t sys_start(caller* function);
uint64_t sys_child(caller* function);
bool sys_exit();
bool sys_kill();
bool sys_pause(uint64_t pid);
bool sys_resume(uint64_t pid);
uint64_t sys_getPid();
bool sys_hasChild();
int sys_getKey(uint8_t fd, int* buffer, size_t count);

#endif