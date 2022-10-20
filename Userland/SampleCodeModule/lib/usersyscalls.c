#include <usersyscalls.h>

int sys_read(uint8_t fd, char *buffer, size_t count)
{
  return _syscall(SYS_READ, fd, (uint64_t)buffer, 1);
}

int sys_write(uint8_t fd, const char *buffer, size_t count)
{
    return _syscall(SYS_WRITE, fd, (uint64_t)buffer, count);
}

int sys_clear(uint8_t fd)
{
    return _syscall(SYS_CLEAN_SCREEN, fd, 0, 0);
}

int sys_inforeg(uint64_t *buffer)
{
  return _syscall(SYS_INFOREG, 0, (uint64_t)buffer,0);
}

int sys_dateAndTime(uint64_t id)
{
  return _syscall(SYS_DATENTIME, (uint64_t)id, 0, 0);
}

int sys_printMem(uint64_t direc, uint8_t * buffer, uint64_t bytes)
{
  return _syscall(SYS_PRINTMEM,(uint64_t)direc, (uint64_t) buffer, (uint64_t)bytes);
}

int sys_wait(uint64_t seconds)
{
  return _syscall(SYS_WAIT, (uint64_t)seconds, 0, 0);
}

// Muestra cursor, 0 para desactivar 1 para activar.
int sys_showCursor(int active)
{
  return _syscall(SYS_SET_CURSOR, (uint64_t)active, 0, 0);
}

int sys_getKey(uint8_t fd, int* buffer, size_t count)
{
  return _syscall(SYS_GETKEY, fd, (uint64_t)buffer, 1);
}

void *sys_malloc(size_t memory)
{
  return (void *)_syscall(SYS_MALLOC, memory, 0, 0);
}

void sys_free(void *memory)
{
 _syscall(SYS_FREE, (uint64_t) memory, 0, 0);
}

void sys_memDump()
{
 _syscall(SYS_MEMDUMP, 0, 0, 0);
}

int sys_getpid()
{
  return _syscall(SYS_GETPID, 0, 0, 0);
}

void sys_printProcesses()
{
  _syscall(SYS_PRINTPROCESSES, 0, 0, 0);
}
void sys_printProcess(int pid)
{
  _syscall(SYS_PRINTPROCESS, pid, 0, 0);
}
int sys_kill(int pid)
{
  return _syscall(SYS_KILL, pid, 0, 0);
}
void sys_nice(int pid, int newPriority)
{
 _syscall(SYS_NICE, pid, newPriority, 0);
}
void sys_block(int pid)
{
  _syscall(SYS_BLOCK, pid, 0, 0);
}
void sys_resume(int pid)
{
  _syscall(SYS_RESUME, pid, 0, 0);
}
void sys_yield()
{
  _syscall(SYS_YIELD, 0 , 0 , 0);
}