#ifndef IDTLOADER_H
#define IDTLOADER_H

#include <stdint.h>

void configureIDT();
void irqDispatcher(int n);
void load_idt();

#endif