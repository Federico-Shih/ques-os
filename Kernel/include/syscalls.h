#ifndef SYSCALLS_H
#define SYSCALLS_H
#include <stdint.h>
#include <stdlib.h>
#include "console.h"
#include "syscodes.h"

extern uint8_t _getRTCInfo(uint64_t);
extern uint8_t _getMem(uint64_t direc);

uint8_t sys_dateAndTime(uint64_t rtcID);
int sys_inforeg(uint64_t *buffer);
void snapshotRegisters(uint64_t* rsp);
void sys_getMem(uint64_t direc, uint8_t * buffer, uint64_t bytes);
uint64_t syscallHandler(syscall_id rax, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);
void sys_wait(uint64_t seconds);

#endif