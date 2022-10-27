#include <usersyscalls.h>

int sys_read(uint8_t fd, char *buffer, size_t count)
{
  return _syscall(SYS_READ, fd, (uint64_t)buffer, 1, 0, 0);
}

int sys_write(uint8_t fd, const char *buffer, size_t count)
{
    return _syscall(SYS_WRITE, fd, (uint64_t)buffer, count, 0, 0);
}

int sys_clear(uint8_t fd)
{
    return _syscall(SYS_CLEAN_SCREEN, fd, 0, 0, 0, 0);
}

int sys_inforeg(uint64_t *buffer)
{
  return _syscall(SYS_INFOREG, 0, (uint64_t)buffer,0, 0, 0);
}

int sys_dateAndTime(uint64_t id)
{
  return _syscall(SYS_DATENTIME, (uint64_t)id, 0, 0, 0, 0);
}

int sys_printMem(uint64_t direc, uint8_t * buffer, uint64_t bytes)
{
  return _syscall(SYS_PRINTMEM,(uint64_t)direc, (uint64_t) buffer, (uint64_t)bytes, 0, 0);
}

int sys_wait(uint64_t seconds)
{
  return _syscall(SYS_WAIT, (uint64_t)seconds, 0, 0, 0, 0);
}

// Muestra cursor, 0 para desactivar 1 para activar.
int sys_showCursor(int active)
{
  return _syscall(SYS_SET_CURSOR, (uint64_t)active, 0, 0, 0, 0);
}

int sys_getKey(uint8_t fd, int* buffer, size_t count)
{
  return _syscall(SYS_GETKEY, fd, (uint64_t)buffer, 1, 0, 0);
}

// Syscalls de manejo de memoria (heap)

void *sys_malloc(size_t memory)
{
  return (void *)_syscall(SYS_MALLOC, memory, 0, 0, 0, 0);
}

void sys_free(void *memory)
{
 _syscall(SYS_FREE, (uint64_t) memory, 0, 0, 0, 0);
}

void sys_memDump()
{
 _syscall(SYS_MEMDUMP, 0, 0, 0, 0, 0);
}

// Syscalls de manejo de procesos

int sys_create()
{
  return 0;
}

int sys_getpid()
{
  return _syscall(SYS_GETPID, 0, 0, 0, 0, 0);
}

void sys_printProcesses()
{
  _syscall(SYS_PRINTPROCESSES, 0, 0, 0, 0, 0);
}

void sys_printProcess(int pid)
{
  _syscall(SYS_PRINTPROCESS, pid, 0, 0, 0, 0);
}

int sys_kill(int pid)
{
  return _syscall(SYS_KILL, pid, 0, 0, 0, 0);
}

void sys_nice(int pid, int newPriority)
{
 _syscall(SYS_NICE, pid, newPriority, 0, 0, 0);
}

int sys_block(int pid)
{
  return _syscall(SYS_BLOCK, pid, 0, 0, 0, 0);
}

int sys_resume(int pid)
{
  return _syscall(SYS_RESUME, pid, 0, 0, 0, 0);
}

void sys_yield()
{
  _syscall(SYS_YIELD, 0 , 0 , 0, 0, 0);
}
int sys_startTask(void (*process)(unsigned int argc, char **argv), int argc, char **argv, int foreground)
{
  return _syscall(SYS_CREATE_PROCESS, (uint64_t)process, (uint64_t)argc, (uint64_t)argv, (uint64_t)foreground, 0);
}

// Syscalls de manejo de semaforos

int sys_semOpen(char *id, int initialValue)
{
  return _syscall(SYS_SEMOPEN, (uint64_t)id, (uint64_t)initialValue, 0, 0, 0);
}

int sys_semInit(int initialValue)
{
  return _syscall(SYS_SEMINIT, initialValue, 0, 0, 0, 0);
}

int sys_semWait(int id)
{
  return _syscall(SYS_SEMWAIT, id, 0, 0, 0, 0);
}

int sys_semPost(int id)
{
  return _syscall(SYS_SEMPOST, id, 0, 0, 0, 0);
}
int sys_semClose(int id)
{
  return _syscall(SYS_SEMCLOSE, id, 0, 0, 0, 0);
}
void sys_printSemInfo()
{
  _syscall(SYS_SEMPRINT, 0, 0, 0, 0, 0);
}

// Syscalls de manejo de pipes

int sys_pipeOpen(int pipeId)
{
  return _syscall(SYS_PIPEOPEN, pipeId, 0, 0, 0, 0);
}
int sys_pipeRead(int pipeId)
{
  return _syscall(SYS_PIPEREAD, pipeId, 0, 0, 0, 0);
}
int sys_pipeWrite(int pipeId, char *str)
{
  return _syscall(SYS_PIPEWRITE, (uint64_t)pipeId, (uint64_t)str, 0, 0, 0);
}
int sys_pipeClose(int pipeId)
{
  return _syscall(SYS_PIPECLOSE, pipeId, 0, 0, 0, 0);
}
void sys_printPipeInfo()
{
  _syscall(SYS_PIPEPRINT, 0, 0, 0, 0, 0);
}