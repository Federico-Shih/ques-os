// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include "string.h"
#include "lib.h"
#include "moduleLoader.h"
#include "idtLoader.h"
#include "console.h"
#include "naiveConsole.h"
#include "keyboard.h"
#include "lib.h"
#include "memoryManager.h"
#include "processManager.h"
#include "interrupts.h"
#include "sem.h"
#include "pipes.h"
#include "keyboard.h"
#include "initProcess.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;
static void *const memoryManagerModuleAddress = (void *)0x600000;

#define HEAP_SIZE 0x100000

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void *getStackBase()
{
	return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
									- sizeof(uint64_t)										// Begin at the top of the stack
	);
}

void *initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(_cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void *moduleAddresses[] = {
			sampleCodeModuleAddress,
			sampleDataModuleAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

int main()
{

	initializeMemoryManager(memoryManagerModuleAddress, HEAP_SIZE);
	
	// ncPrint("[Kernel Main]");
	// ncNewline();
	// ncPrint("  Sample code module at 0x");
	// ncPrintHex((uint64_t)sampleCodeModuleAddress);
	// ncNewline();

	// ncPrint("  Sample data module at 0x");
	// ncPrintHex((uint64_t)sampleDataModuleAddress);
	// ncNewline();
	// ncPrint("  Sample data module contents: ");
	// ncPrint((char*)sampleDataModuleAddress);
	// ncNewline();
	initializeMemoryManager(memoryManagerModuleAddress, HEAP_SIZE);

	initSemSystem();
	initPipeSystem();
	initKeyboardSystem();
	initScheduler();
	ncClear();

	char *args[] = {"Init userland"};
	int userlandFd[2] = {0, 1};
	int userlandPid = startTask(sampleCodeModuleAddress, 1, args, 1, userlandFd);
	setUserlandPid(userlandPid);

	// Activo interrupciones para empezar el scheduler
	load_idt();
	_hlt(); // Fuerzo a que el scheduler empiece
	printline("System failure.\n");
	return 0;
}
