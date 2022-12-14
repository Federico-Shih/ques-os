#ifndef COMMANDS_H
#define COMMANDS_H
#define DUMP_SIZE 33
#define LAST_MEM 0x80000000
#define MAX_HELP_LENGTH 250
#define HELP_PAGE 8

#include <stdbool.h>

/*
  Los programas validos son estos:
  Que reciban la cantidad de variables, los argumentos en un array de strings
  Tienen que devolver si termino su ejecucion o no. 1 si termino, 0 sino.
*/

typedef struct
{
  char *name;
  void (*runner)(unsigned int count, char **args);
  char help[MAX_HELP_LENGTH];
} command_t;

command_t *getCommand(char *commandName);

/*
 Lista de comandos y su estructura
 command_t {
   char* name;        NOMBRE DE LA FUNCION
   void (*runner)();  FUNCION A CORRER
 }
*/
extern int _opcodeExp();

void dateAndTime();
void help(unsigned int argc, char *argv[]);
void infoReg();
void holaMundo();
void printmem(unsigned int argc, char *argv[]);
void fibonacci();
void primes();
char *getDescriptions(char *function);
void killTask(unsigned int argc, char *argv[]);
void blockTask(unsigned int argc, char *argv[]);
void resumeTask(unsigned int argc, char *argv[]);
void niceTask(unsigned int argc, char *argv[]);
void cat(unsigned int argc, char *argv[]);
void wc(unsigned int argc, char *argv[]);
void filter(unsigned int argc, char *argv[]);
void printCheeseOs();
void printCheese();
void printShortcuts();
void ejemplos();
void echo(unsigned int argc, char *argv[]);
void loop();

#endif