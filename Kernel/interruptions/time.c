// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "time.h"
#include "console.h"

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
	blinkCursor();
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}
