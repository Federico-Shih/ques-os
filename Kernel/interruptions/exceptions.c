
#include <console.h>
#include <syscalls.h>
#include <naiveConsole.h>
#include <scheduler.h>
#include <snapshot.h>
#include <string.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6
#define TOTAL_REGISTERS 17

static void zero_division();
static void invalid_opcode();

void exceptionDispatcher(int exception, uint64_t rsp) {
	if (exception == ZERO_EXCEPTION_ID){
		zero_division();
	}else if(exception == INVALID_OPCODE_ID){
		invalid_opcode();
	}
	printRegisters((uint64_t*) rsp);
	uint64_t currentPid = getPid();
	killTask(currentPid);
}

static void invalid_opcode(){
	char * msg = "\nException: invalid operand code.\n";
	sys_write(STDERR, msg , strlen(msg));
}

static void zero_division() {
	char * msg = "\nException: division by zero.\n";
	sys_write(STDERR, msg, strlen(msg)); 
}

