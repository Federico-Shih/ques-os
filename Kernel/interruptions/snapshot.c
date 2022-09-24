#include <snapshot.h>
#include <syscalls.h>
#include <console.h>
#include <string.h>

static char* registerNames[] = {
  "R15: ", "R14: ", "R13: ", "R12: ", "R11: ", "R10: ",
  "R9:  ", "R8:  ", "RSI: ", "RDI: ", "RBP: ", "RDX: ",
  "RCX: ", "RBX: ", "RAX: ", "RIP: ", "RSP: "
};

static uint64_t registers[TOTAL_REGISTERS] = {0};

void snapshotRegisters(uint64_t* rsp){
    for(int i = 0 ; i < TOTAL_REGISTERS ; i++){
        registers[i] = rsp[i];
        if (i == 16) {
            // Debido a que cuando se armo el stackframe se pusheo rsp, aca estamos accediendo forzosamente al rsp correcto
            registers[i] = rsp[i + 0x2];
        }
    }
}

int loadRegisters(uint64_t *buffer){
    for(int i = 0 ; i < TOTAL_REGISTERS ; i++){ //buffer[0] = r15, ... buffer[15] = rax
        buffer[i] = registers[i];
    }
    return 0;
}

void printRegisters(uint64_t *rsp){
	for(int i = 0; i < TOTAL_REGISTERS; i++){
		sys_write(STDOUT, registerNames[i], strlen(registerNames[i]));
		printBase(rsp[i],16);
		printChar('\n');
	}
	printChar('\n');
}