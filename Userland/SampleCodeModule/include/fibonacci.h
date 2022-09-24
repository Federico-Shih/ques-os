#ifndef FIBO
#define FIBO

//defino fiboStruct y fiboPointer
typedef struct{
  uint64_t n_2;
  uint64_t n_1;
  int cantPrinted;
}fiboS;

typedef fiboS * fiboP;

void initFibonacci(int screenId);
void fibonacci();

#endif