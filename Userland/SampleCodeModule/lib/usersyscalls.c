#include <usersyscalls.h>

int sys_read(uint8_t fd, char *buffer, size_t count) {
  return _syscall(SYS_READ_ID, fd, (uint64_t)buffer, 1);
}

int sys_write(uint8_t fd, const char *buffer, size_t count) {
    return _syscall(SYS_WRITE_ID, fd, (uint64_t)buffer, count);
}

int sys_clear(uint8_t fd) {
    return _syscall(SYS_CLEAR_ID, fd, 0, 0);
}

int sys_inforeg(uint64_t *buffer){
  return _syscall(SYS_INFOREG_ID, 0, (uint64_t)buffer,0);
}

int sys_dateAndTime(uint64_t id){
  return _syscall(SYS_DNT_ID, (uint64_t)id, 0, 0);
}

int sys_printMem(uint64_t direc, uint8_t * buffer, uint64_t bytes){
  return _syscall(SYS_PRINTMEM_ID,(uint64_t)direc, (uint64_t) buffer, (uint64_t)bytes);
}

int sys_wait(uint64_t seconds){
  return _syscall(SYS_WAIT_ID, (uint64_t)seconds, 0, 0);
}

// Muestra cursor, 0 para desactivar 1 para activar.
int sys_showCursor(int active) {
  return _syscall(SYS_CURSOR_ID, (uint64_t)active, 0, 0);
}

// Cambia pantalla, 0 para completa, 1 para izq, 2 para der
int sys_switchScreen(size_t screen) {
  return _syscall(SYS_SWITCHSCREEN_ID, (uint64_t)screen, 0, 0);
}

// Cambia pantalla unica a pantalla doble
// mode = 0 es unica, mode = 1 es doble
int sys_toggleMode(int mode) {
  return _syscall(SYS_TOGGLEMODE_ID, (uint64_t)mode, 0, 0);
}

uint64_t sys_start(caller* function){
  return _syscall(SYS_START_ID, (uint64_t)function, 0, 0);
}

uint64_t sys_child(caller* function){
  return _syscall(SYS_CHILD_ID, (uint64_t)function, 0, 0);
}

bool sys_exit(){
  return _syscall(SYS_EXIT_ID, 0, 0, 0);
}

bool sys_kill(uint64_t pid){
  return _syscall(SYS_KILL_ID, pid, 0, 0);
}

bool sys_pause(uint64_t pid){
  return _syscall(SYS_PAUSE_ID, pid, 0, 0);
}

bool sys_resume(uint64_t pid){
  return _syscall(SYS_RESUME_ID, pid, 0, 0);
}

uint64_t sys_getPid(){
  return _syscall(SYS_GETPID_ID, 0, 0, 0);
}

bool sys_hasChild() {
  return _syscall(SYS_HASCHILD, 0, 0, 0);
}

int sys_getKey(uint8_t fd, int* buffer, size_t count) {
  return _syscall(SYS_GETKEY_ID, fd, (uint64_t)buffer, 1);
}