#include "printinfo.h"
#include "ustdlib.h"
#include "usersyscalls.h"

// ----------------- DECLARACIONES STATIC ------------------------
static void printUserlandSem(userlandSem sem);
static void printBlockedPids(userlandBlockedPids * blockedPids);
static void freeSemInfo(userlandSemInfo * info);
static void freeUserlandSem(userlandSem sem);
static void freeBlockedPids(userlandBlockedPids * blockedPids);

static void freePipeInfo(userlandPipeInfo * info);
static void freePipe(userlandPipe pipe);

static void printPipe(userlandPipe pipe, int buffersize);

// ----------------- SEMAFOROS ---------------------------

static void printBlockedPids(userlandBlockedPids * blockedPids){
    if(blockedPids->length == 0){
        _fprintf("[No hay procesos bloqueados]\n");
        return;
    }

    for (int i = 0; i < blockedPids->length; i++){
        _fprintf("%d, ", blockedPids->array[i]);
        _fprintf("\b\b\n");
    }
}

static void printUserlandSem(userlandSem sem){
    _fprintf("Id: %d | ", sem.id);
    _fprintf("Valor: %d | ", (int)sem.value);
    _fprintf("Nombre del semaforo: %s | ", _strcasecmp(sem.name, "") ? sem.name : "[Sin nombre]");
    _fprintf("Num de procesos vinculados: %d | ", (int)sem.attachedProcesses);
    _fprintf("IDs de procesos bloqueados: ");
    printBlockedPids(sem.blockedPids);
}

void printSemInfo(){
    userlandSemInfo * info = sys_getSemInfo();
    for (int i = 0; i < info->length; i++){
        printUserlandSem(info->array[i]);
    }
    freeSemInfo(info);
}

static void freeBlockedPids(userlandBlockedPids * blockedPids){
    sys_free(blockedPids->array);
    sys_free(blockedPids);
}

static void freeUserlandSem(userlandSem sem){
    sys_free(sem.name);
    freeBlockedPids(sem.blockedPids);
}

static void freeSemInfo(userlandSemInfo * info){
    for (int i = 0; i < info->length; i++){
        freeUserlandSem(info->array[i]);
    }
    sys_free(info->array);
    sys_free(info);
}

// ------------------ PIPES ---------------------------
static void freePipe(userlandPipe pipe){
    freeUserlandSem(*pipe.writeSem);
    freeUserlandSem(*pipe.readSem);
    sys_free(pipe.writeSem);
    sys_free(pipe.readSem);
}

static void freePipeInfo(userlandPipeInfo * info){
    for(int i = 0; i < info->length; i++)
        freePipe(info->array[i]);
    sys_free(info->array);
    sys_free(info);
}

static void printPipe(userlandPipe pipe, int buffersize){
    _fprintf("Pipe ID: %d\n", pipe.id);
    _fprintf("ID semaforo de lectura: %d\n", pipe.readSem->id);
    _fprintf("ID semaforo de escritura: %d\n", pipe.writeSem->id);
    _fprintf("Procesos con el semaforo abierto: %d\n", pipe.totalProcesses);
    _fprintf("Lista de procesos bloqueados:\n");
    _fprintf("\tBloqueados para escribir: ");
    printBlockedPids(pipe.writeSem->blockedPids);
    _fprintf("\b\b\n\tBloqueados para leer: ");
    printBlockedPids(pipe.readSem->blockedPids);
    _fprintf("Cantidad de caracteres disponibles para leer: %d\n", ((pipe.writeIndex - pipe.readIndex)%buffersize));
}

void printPipeInfo(){
    userlandPipeInfo * info = sys_getPipeInfo();
    for (int i = 0; i < info->length; i++)
    {
        printPipe(info->array[i], info->pipeBufferSize);
    }
    freePipeInfo(info);
}