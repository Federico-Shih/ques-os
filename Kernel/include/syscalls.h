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
  // Semaphores
  SYS_SEMOPEN,
  SYS_SEMWAIT,
  SYS_SEMPOST,
  SYS_SEMCLOSE,
  SYS_SEMPRINT
} syscall_id;


extern uint8_t _getRTCInfo(uint64_t);
extern uint8_t _getMem(uint64_t direc);

uint8_t sys_dateAndTime(uint64_t rtcID);
int sys_inforeg(uint64_t *buffer);
void snapshotRegisters(uint64_t* rsp);
void sys_getMem(uint64_t direc, uint8_t * buffer, uint64_t bytes);
int sys_read(FILE_DESCRIPTOR fd, char* buffer, size_t count);
int sys_write(FILE_DESCRIPTOR fd, const char* buffer, uint64_t size);
uint64_t syscallHandler(syscall_id rax, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);
void sys_wait(uint64_t seconds);

#endif