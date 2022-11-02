#ifndef COMMANDS_H
#define COMMANDS_H
#define DUMP_SIZE 34
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

command_t *getCommands();
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
// TODO: ARREGLAR
void printmem(unsigned int argc, char *argv[]);
void divZero();
void invalidOpcode();
void fibonacci();
void primes();
char *getDescriptions(char *function);
void clearScreen();
void kill(unsigned int argc, char *argv[]);
void block(unsigned int argc, char *argv[]);
void resume(unsigned int argc, char *argv[]);
void nice(unsigned int argc, char *argv[]);
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