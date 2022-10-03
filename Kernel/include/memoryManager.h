#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

void* malloc(uint64_t);

void free(void* memory);

void initializeMemoryManager (void* heap_base, unsigned int heap_size);

void memoryDump();

#endif