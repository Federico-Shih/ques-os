#ifndef SYSCALLS_H
#define SYSCALLS_H
#include <stdint.h>
#include <stdlib.h>
#include <console.h>

typedef enum {
  SYS_READ=0, 
  SYS_WRITE, 
  SYS_CLEAN_SCREEN, 
  SYS_INFOREG, 
  SYS_DATENTIME, 
  SYS_PRINTMEM, 
  SYS_SET_CURSOR, 
  SYS_SET_SCREEN, 
  SYS_TOGGLE_MODE,
  SYS_START,
  SYS_CHILD,
  SYS_EXIT,
  SYS_KILL,
  SYS_PAUSE,
  SYS_RESUME,
  SYS_GETPID,
  SYS_HASCHILD,
  SYS_WAIT,
  SYS_GETKEY
} syscall_id;


extern uint8_t _getRTCInfo(uint64_t);
extern uint8_t _getMem(uint64_t direc);

uint8_t sys_dateAndTime(uint64_t rtcID);
int sys_inforeg(uint64_t *buffer);
void snapshotRegisters(uint64_t* rsp);
void sys_getMem(uint64_t direc, uint8_t * buffer, uint64_t bytes);
int sys_read(FILE_DESCRIPTOR fd, char* buffer, size_t count);
int sys_write(FILE_DESCRIPTOR fd, const char* buffer, uint64_t size);
uint64_t syscallHandler(syscall_id rax, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t rsp);
void sys_wait(uint64_t seconds);

#endif