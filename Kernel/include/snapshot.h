#ifndef SNAPSHOT_H
#define SNAPSHOT_H
#include <stdint.h>
#define TOTAL_REGISTERS 17

// Graba el rsp actual
void snapshotRegisters(uint64_t* rsp);

// Guarda el snapshot en el buffer
int loadRegisters(uint64_t *buffer);

// Imprimir los registros
void printRegisters(uint64_t* rsp);

#endif