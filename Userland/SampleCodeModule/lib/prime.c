#include <ustdlib.h>
#include <prime.h>
#include <usersyscalls.h>

int _sqrt(int x) {
  float xhalf = 0.5f * x;
  union {
    float x;
    int i;
  } u;

  u.x = x;
  u.i = 0x5f375a86 - (u.i >> 1);
  u.x = u.x * (1.5f - xhalf * u.x * u.x);
  return ((int)(u.x * x)) + 2; 
}

bool isPrime(int num){
  int numsqrt = _sqrt(num);
  int isPrime = 1;
  if(num == 2 || num == 3)
    return 1;
  for (int i = 3; i <= numsqrt && isPrime; i+=2){
    if(num%i == 0)
      isPrime = 0;
  }
  return isPrime;
}

void primes(){
  int num = 2;
  _fprintf(STDOUT, "prime : %d\n",num);
  num++;
  sys_wait(1);
  while(1){
    if (isPrime(num)) {
      _fprintf(STDOUT, "prime : %d\n", num);
      sys_wait(1);
    }
    num += 2;
  }
  sys_exit();
}

