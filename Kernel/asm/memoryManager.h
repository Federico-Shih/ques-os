#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>

#include <stdlib.h>

void* malloc();

void free(void* memory);

void initializeMemoryManager (void* heap_base, unsigned int heap_size);

void memoryDump();

#endif