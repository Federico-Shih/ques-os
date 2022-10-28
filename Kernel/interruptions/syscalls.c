#include <syscalls.h>
#include <keyboard.h>
#include <snapshot.h>
#include <console.h>
#include <time.h>
#include <interrupts.h>
#include <memoryManager.h>
#include <processManager.h>
#include <sem.h>
#include "pipes.h"


uint8_t sys_dateAndTime(uint64_t rtcID){
	uint8_t x = _getRTCInfo(rtcID);
	uint8_t result = ((x / 16) * 10) + (x & 0xf);
	return result;
}

void sys_wait(uint64_t seconds){
    int startingSeconds = seconds_elapsed();
    int currentSeconds = startingSeconds;
    while((currentSeconds - startingSeconds) <= seconds){
        yield();
        _hlt();
        currentSeconds = seconds_elapsed();
    }
}

int sys_inforeg(uint64_t *buffer){
    loadRegisters(buffer);
    return 0;
}


void sys_getMem(uint64_t direc, uint8_t * buffer, uint64_t bytes){
    for (uint8_t i = 0; i < bytes; i++) {
        buffer[i] = (uint8_t) _getMem(direc+i);     // llamo 'bytes'(32) veces a _getMem con 32 posiciones de memoria distintos
    }
}


char sys_read(){
    return readStdin();
}

int sys_write(FILE_DESCRIPTOR fd, const char* buffer, uint64_t size) {
    if (buffer == 0 || size == 0 || fd > 4) {
        return -1;
    }

    color_t fontColor = (fd == STDERR) ? (RED) : (LGREY);
    int i;
    for (i = 0; i < size && buffer[i]; i += 1) {
        printCharColor(buffer[i], fontColor, BLACK, 1);
    }
    return i;
}


uint64_t syscallHandler(syscall_id rax, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4) {
    switch (rax) {
        case SYS_READ:
            return sys_read((char *)arg0);
        case SYS_WRITE:
            return sys_write((FILE_DESCRIPTOR)arg0, (char *)arg1, (size_t)arg2);
        case SYS_CLEAN_SCREEN:
            clearScreen((FILE_DESCRIPTOR)arg0);
            return 0;
        case SYS_INFOREG:
            return sys_inforeg((uint64_t *)arg1);
        case SYS_DATENTIME:
            return sys_dateAndTime((uint64_t) arg0);
        case SYS_PRINTMEM:
            sys_getMem((uint64_t)arg0, (uint8_t *)arg1, (uint64_t) arg2);
            return 0;
        case SYS_SET_CURSOR:
            setCursor((int)arg0);
            return 0;
        case SYS_WAIT:
            sys_wait((uint64_t)arg0);
            return 0;
        case SYS_MALLOC:
            return (uint64_t)malloc(arg0);
        case SYS_FREE:
            free((void *) arg0);
            return 0;
        case SYS_MEMDUMP:
            memoryDump();
            return 0;
        case SYS_CREATE_PROCESS:
            return startTask((void (*)(int argc, char **argv))arg0, (int) arg1, (char **)arg2, (int)arg3);
        case SYS_GETPID:
            return getpid();
        case SYS_PRINTPROCESSES:
            printTasks();
            return 0;
        case SYS_PRINTPROCESS:
            printTask((int) arg0);
            return 0;
        case SYS_KILL:
            return terminateTask((int) arg0);
        case SYS_EXIT:
            return killCurrent();
        case SYS_NICE:
            nice((int) arg0, (int) arg1);
            return 0;
        case SYS_BLOCK:
            blockTask((int) arg0);
            return 0;
        case SYS_RESUME:
            resumeTask((int) arg0);
            return 0;
        case SYS_YIELD:
            yield();
            return 0;
        case SYS_WAITPID:
            return waitpid((int) arg0);
        case SYS_SEMOPEN:
            return semOpen((char *) arg0, (int) arg1);
        case SYS_SEMINIT:
            return semInit((int)arg0);
        case SYS_SEMWAIT:
            return semWait((int) arg0);
        case SYS_SEMPOST:
            return semPost((int) arg0);
        case SYS_SEMCLOSE:
            return semClose((int) arg0);
        case SYS_SEMPRINT:
            printSemInfo();
            return 0;
        case SYS_PIPEOPEN:
            return pipeOpen((int) arg0);
        case SYS_PIPECLOSE:
            return pipeClose((int) arg0);
        case SYS_PIPEREAD:
            return pipeRead((int) arg0);
        case SYS_PIPEWRITE:
            return pipeWrite((int) arg0, (char *) arg1);
        case SYS_PIPEPRINT:
            printPipeInfo();
            return 0;
    }
    return -1;
}
