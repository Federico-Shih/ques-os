#include <syscalls.h>
#include <keyboard.h>
#include <snapshot.h>
#include <console.h>
#include <scheduler.h>
#include <time.h>
#include <interrupts.h>


uint8_t sys_dateAndTime(uint64_t rtcID){
	uint8_t x = _getRTCInfo(rtcID);
	uint8_t result = ((x / 16) * 10) + (x & 0xf);
	return result;
}

void sys_wait(uint64_t seconds){
    int startingSeconds = seconds_elapsed();
    int currentSeconds = startingSeconds;
    while((currentSeconds - startingSeconds) <= seconds){
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


int sys_read(FILE_DESCRIPTOR fd, char* buffer, size_t count){
    if (buffer == 0 || count == 0 || fd != 0) return -1;

    uint8_t i = 0;
    int c;
    while (i < count && (c = getFromBuffer()) != -1) {
        buffer[i++] = c;
    }
    return i;
}

int sys_readKey(FILE_DESCRIPTOR fd, int* buffer, size_t count){
    if (buffer == 0 || count == 0 || fd != 0) return -1;

    int i = 0;
    int c;
    while (i < count && (c = getFromBuffer()) != -1) {
        buffer[i++] = c;
    }
    return i;
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

uint64_t syscallHandler(syscall_id rax, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t rsp) {
    switch (rax) {
        case SYS_READ:
            toggleBuffer(CHARBUFFER);
            return sys_read((FILE_DESCRIPTOR)arg0, (char *)arg1, (size_t)arg2);
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
        case SYS_SET_SCREEN:
            switchScreens((size_t)arg0);
            return 0;
        case SYS_TOGGLE_MODE:
            if (!(size_t)arg0) {
                initializeSingleScreen();
            } else {
                initializeDualScreen();
            }
            return 0;
        case SYS_START:
            startTask((caller *)arg0, rsp);
            return 0;
        case SYS_CHILD:
            return startChild((caller *)arg0);
        case SYS_EXIT:
            return killTask(getPid());
        case SYS_KILL:
            return killTask(arg0);
        case SYS_PAUSE:
            return pauseTask(arg0);
        case SYS_RESUME:
            return resumeTask(arg0);
        case SYS_GETPID:
            return getPid();
        case SYS_HASCHILD:
            return currentHasChilds();
        case SYS_WAIT:
            sys_wait((uint64_t)arg0);
            return 0;
        case SYS_GETKEY:
            toggleBuffer(SCANCODEBUFFER);
            return sys_readKey((FILE_DESCRIPTOR)arg0, (int*)arg1, (size_t)arg2);
    }
    return -1;
}
