#include <ustdlib.h>
#include <fibonacci.h>
#include <usersyscalls.h>

void fibonacci(){
  uint64_t a = 0;
  uint64_t b = 1;
  uint64_t c;
  int count = 2;
  _fprintf(STDOUT, "n %d: %u\n", 1, 0);
  sys_wait(1);
  _fprintf(STDOUT, "n %d: %u\n", 2, 1);
  while(1){
    c = a + b;
    a = b;
    b = c;
    count++;
    sys_wait(1);
    _fprintf(STDOUT,"n %d: %u\n",count ,c);
  }
  sys_exit();
}
