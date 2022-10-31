// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <time.h>
#include <keyboard.h>
#include <stdint.h>
#include <naiveConsole.h>

#define KEYBOARD 1
#define TIMER 0

void irqDispatcher(uint64_t irq, uint64_t rsp) {
	switch (irq) {
		case TIMER:
			timer_handler();
			break;
		case KEYBOARD:
			keyboardHandler(rsp);
			break;
	}
}