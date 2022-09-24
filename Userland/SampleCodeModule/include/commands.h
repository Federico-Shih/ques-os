#ifndef COMMANDS_H
#define COMMANDS_H
#define COMMANDS_LENGTH 10
#define DUMP_SIZE 32
#define LAST_MEM 0x80000000

#include <stdbool.h>

/* 
  Los programas validos son estos:
  Que reciban la cantidad de variables, los argumentos en un array de strings
  Tienen que devolver si termino su ejecucion o no. 1 si termino, 0 sino.
*/ 

typedef struct {
  char *name;
  void (*runner)(int count, void** args);
} command;

command* getCommands();

/*
 Lista de comandos y su estructura
 command {
   char* name;        NOMBRE DE LA FUNCION
   void (*runner)();  FUNCION A CORRER
 } 
*/
extern int _opcodeExp();

void dateAndTime();
void help(int argc, char* argv[]);
void infoReg();
void holaMundo();
void printmem(int argc, char* argv[]);
void divZero();
void invalidOpcode();
void fibonacci();
void primes();
char* getDescriptions(char* function);
void clearScreen();

#endif