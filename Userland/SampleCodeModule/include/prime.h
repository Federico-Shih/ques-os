#ifndef PRIME
#define PRIME

typedef struct{
  int cantPrinted;
  int lastPrime;
}primeS;

typedef primeS * primeP;

void initPrime(int screenId);
//int prime(int argc, char* argv[], int screenId);
void primes();

#endif