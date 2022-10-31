// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscalls.h>
#include <naiveConsole.h>
#include <snapshot.h>
#include <string.h>
#include <processManager.h>
#include "stdio.h"

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6
#define TOTAL_REGISTERS 17

static void zero_division();
static void invalid_opcode();

color_t error[] = { RED, BLACK };

void exceptionDispatcher(int exception, uint64_t rsp) {
	if (exception == ZERO_EXCEPTION_ID){
		zero_division();
	}else if(exception == INVALID_OPCODE_ID){
		invalid_opcode();
	}
	printRegisters((uint64_t*) rsp);
	killCurrent();
}

static void invalid_opcode(){
	char * msg = "\nException: invalid operand code.\n";
	sysWrite(msg , strlen(msg), error);
}

static void zero_division() {
	char * msg = "\nException: division by zero.\n";
	sysWrite(msg , strlen(msg), error);
}

