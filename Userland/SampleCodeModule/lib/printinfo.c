#include "printinfo.h"
#include "ustdlib.h"
#include "usersyscalls.h"

// ----------------- DECLARACIONES STATIC ------------------------
static void printUserlandSem(userlandSem sem);
static void printBlockedPids(userlandBlockedPids *blockedPids);
static void freeSemInfo(userlandSemInfo *info);
static void freeUserlandSem(userlandSem sem);
static void freeBlockedPids(userlandBlockedPids *blockedPids);

static void freePipeInfo(userlandPipeInfo *info);
static void freePipe(userlandPipe pipe);

static void printPipe(userlandPipe pipe, int buffersize);

// ----------------- SEMAFOROS ---------------------------

static void printBlockedPids(userlandBlockedPids *blockedPids)
{
    if (blockedPids->length == 0)
    {
        _fprintf("[No hay procesos bloqueados]\n");
        return;
    }

    for (int i = 0; i < blockedPids->length; i++)
    {
        _fprintf("%d, ", blockedPids->array[i]);
        _fprintf("\b\b");
    }
}

static void printUserlandSem(userlandSem sem)
{
    _fprintf("%d | ", sem.id);
    _fprintf("%d | ", (int)sem.value);
    _fprintf("%s | ", _strcasecmp(sem.name, "") ? sem.name : "[Sin nombre]");
    _fprintf("%d | ", (int)sem.attachedProcesses);
    printBlockedPids(sem.blockedPids);
}

void printSemInfo()
{
    _fprint("ID | Valor | Nombre | # procesos vinculados | procesos bloqueados\n");
    userlandSemInfo *info = getSemInfo();

    if(info == NULL)
    {
        _fprintf("Error al recuperar informacion de los semaforos");
        return;
    }

    for (int i = 0; i < info->length; i++)
    {
        printUserlandSem(info->array[i]);
    }
    freeSemInfo(info);
}

static void freeBlockedPids(userlandBlockedPids *blockedPids)
{
    free(blockedPids->array);
    free(blockedPids);
}

static void freeUserlandSem(userlandSem sem)
{
    free(sem.name);
    freeBlockedPids(sem.blockedPids);
}

static void freeSemInfo(userlandSemInfo *info)
{
    for (int i = 0; i < info->length; i++)
    {
        freeUserlandSem(info->array[i]);
    }
    free(info->array);
    free(info);
}

// ------------------ PIPES ---------------------------
static void freePipe(userlandPipe pipe)
{
    freeUserlandSem(*pipe.writeSem);
    freeUserlandSem(*pipe.readSem);
    free(pipe.writeSem);
    free(pipe.readSem);
}

static void freePipeInfo(userlandPipeInfo *info)
{
    for (int i = 0; i < info->length; i++)
        freePipe(info->array[i]);
    free(info->array);
    free(info);
}

static void printPipe(userlandPipe pipe, int buffersize)
{
    _fprintf("ID: %d | ", pipe.id);
    _fprintf("Read Sem ID: %d | ", pipe.readSem->id);
    _fprintf("Write Sem ID: %d | ", pipe.writeSem->id);
    _fprintf("# procesos de pipe: %d\n", pipe.totalProcesses);
    _fprintf("Procesos bloqueados:\n");
    _fprintf("\tBloqueados para escribir: ");
    printBlockedPids(pipe.writeSem->blockedPids);
    _fprintf("\b\b\n\tBloqueados para leer: ");
    printBlockedPids(pipe.readSem->blockedPids);
    _fprintf("# de caracteres a leer: %d\n", ((pipe.writeIndex - pipe.readIndex) % buffersize));
}

void printPipeInfo()
{
    userlandPipeInfo *info = getPipeInfo();
    if( info == NULL)
    {
        _fprintf("Error al recuperar informacion de los pipes");
        return;
    }
    for (int i = 0; i < info->length; i++)
    {
        printPipe(info->array[i], info->pipeBufferSize);
    }
    freePipeInfo(info);
}

void ps()
{
    schedulerInfo *processes = getSchedulerInfo();
    if (processes == NULL)
    {
        _fprint("ERROR recuperando datos de los procesos.\n");
        return;
    }
    _fprint("Nombre | PID | PPID | Foreground |  RSP  |  RBP  | Prio | Estado\n");
    for (int i = 0; i < processes->length; i += 1)
    {
        processInfo process = processes->array[i];
        _fprintf(
            "%s | %d | %d |  %s  | %x | "
            "%x |  %d  | %s \n",
            process.name, process.pid, process.ppid,
            process.foreground ? "TRUE" : "FALSE", process.rsp,
            process.rbp, process.priority,
            process.state);
    }
    free(processes->array);
    free(processes);
}

void memDump()
{
    memoryInfo *memory = getMemoryInfo();

    if(memory == NULL)
    {
        _fprintf("ERROR recuperando datos de la memoria.\n");
    }

    _fprintf("Informacion actual de la memoria: \n");
    _fprintf("Memoria total: %d\n",memory->totalMem);
    _fprintf("Memorioa libre total: %d\n", memory->availableMem);
    _fprintf("Memoria ocupada total: %d\n", (memory->totalMem - memory->availableMem));

    free(memory);
}