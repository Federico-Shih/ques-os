#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include "shared.h"

void *malloc(uint64_t size);

void free(void *memory);

void initializeMemoryManager(void *heap_base, unsigned int heap_size);

memoryInfo *getMemoryInfo();

#endif