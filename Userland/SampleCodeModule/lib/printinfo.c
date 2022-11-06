// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "printinfo.h"
#include "ustdlib.h"
#include "usersyscalls.h"

// ----------------- DECLARACIONES LOCALES ------------------------
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
        _fprintf("[Sin procesos]");
        return;
    }

    for (int i = 0; i < blockedPids->length; i++)
    {
        if (i != 0)
            _fprint(", ");
        _fprintf("%d", blockedPids->array[i]);
    }
}

static void printUserlandSem(userlandSem sem)
{
    _fprintf("|%-4d|", sem.id);
    _fprintf("%-6d|", (int)sem.value);
    _fprintf("%-19s|", _strcasecmp(sem.name, "") ? sem.name : "[unnamed]");
    _fprintf("%-7d|", (int)sem.attachedProcesses);
    printBlockedPids(sem.blockedPids);
}

void printSemInfo()
{
    _fprint("|ID  |Valor |Nombre            |# PIDs |PIDs bloqueados\n");
    _fprint("---------------------------------------------------------------------------\n");
    userlandSemInfo *info = getSemInfo();

    if(info == NULL)
    {
        _fprintf("Error al recuperar informacion de los semaforos");
        return;
    }

    for (int i = 0; i < info->length; i++)
    {
        printUserlandSem(info->array[i]);
        _putc('\n');
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
    _fprintf("|%-4d|", pipe.id);
    _fprintf("%-11d|", pipe.readSem->id);
    _fprintf("%-12d|", pipe.writeSem->id);
    _fprintf("%-7d|", pipe.totalProcesses);
    printBlockedPids(pipe.writeSem->blockedPids);
    _putc('|');
    printBlockedPids(pipe.readSem->blockedPids);
    // _fprintf("A leer: %d\n", ((pipe.writeIndex - pipe.readIndex) % buffersize));
}

void printPipeInfo()
{
    userlandPipeInfo *info = getPipeInfo();
    if( info == NULL)
    {
        _fprintf("Error al recuperar informacion de los pipes");
        return;
    }
    _fprint("|ID  |ReadSem ID |WriteSem ID |# PIDs |Write PIDs      |Read PIDs      \n");
    _fprint("---------------------------------------------------------------------------\n");
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
    _fprint("|Nombre      |PID    |PPID    |Foreground  |RSP     |RBP     |Prio |Estado|\n");
    _fprint("---------------------------------------------------------------------------\n");
    for (int i = 0; i < processes->length; i += 1)
    {
        processInfo process = processes->array[i];
        _fprintf(
            "|%-12s|%-7d|%-8d|%-12s|%-8x|"
            "%-8x|%-5d|%-5s|\n",
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
        return;
    }

    _fprintf("Informacion actual de la memoria: \n");
    _fprintf("Memoria total: %d\n",memory->totalMem);
    _fprintf("Memorioa libre total: %d\n", memory->availableMem);
    _fprintf("Memoria ocupada total: %d\n", (memory->totalMem - memory->availableMem));

    free(memory);
}