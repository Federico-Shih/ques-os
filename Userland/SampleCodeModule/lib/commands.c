#include <math.h>
#include "commands.h"
#include "ustdlib.h"
#include "stdint.h"
#include "RTCID.h"
#include "usersyscalls.h"
#include "fibonacci.h"
#include "prime.h"
#include "inforeg.h"
#include "test_mm.h"

static command commands[COMMANDS_LENGTH] = {
    {"date&time", &dateAndTime},
    {"divZero", &divZero},
    {"fibonacci", &fibonacci},
    {"hello", &holaMundo},
    {"help", &help},
    {"inforeg", &infoReg},
    {"invalidOpcode", &invalidOpcode},
    {"prime", &primes},
    {"printmem", &printmem},
    {"clear", &clearScreen},
    {"mem", &sys_memDump},
    {"ps", &sys_printProcesses},
    {"kill", &kill},
    {"block", &block},
    {"resume", &resume},
    {"nice", &nice},
    {"test_mm", &test_mm},
    {"semOpen", &sys_semOpen},
    {"semClose", &sys_semClose},
    {"semPost", &sys_semPost},
    {"semRead", &sys_semWait},
    {"printSemInfo", &sys_semOpen},
    {"pipeOpen", &sys_pipeOpen},
    {"pipeClose", &sys_pipeClose},
    {"pipeRead", &sys_pipeRead},
    {"pipeWrite", &sys_pipeWrite},
    {"printPipeInfo", &sys_printPipeInfo},
    
};

static char *commandInfo[COMMANDS_LENGTH] = {
    "date&time : Imprime en patalla la fecha del ano corriente y horario en que fue\nllamado.\n",
    "divzero: Realiza una division por 0. Lanza una excepcion e imprime una captura de los registros al momento de ejecucion.\n",
    "fibonacci: Imprime la secuencia de fibonacci infinitamente hasta que se pause o se termine su ejecucion.\n",
    "hello: Imprime un saludo al usuario.\n",
    "help: Imprime una lista detallada de los comandos  y modulos ejecutables del\nprograma.\n",
    "inforeg: Imprime los registros registros capturados al presionar ctrl + r.\n",
    "invalidOpcode: Lanza la excepcion de invalid operand code e imprime los\nregistros al momento de ejecucion.\n",
    "prime: imprime numeros primos infinitamente hasta que se pause o se termine su\nejecucion.\n",
    "printmem [DIRECCION DE MEMORIA]: Recibe como argumento una direccion de memoria no superior a\n80000000h y luego imprime los proximos 32bytes de memoria adyacentes a la\ndireccion dada.\n",
    "clear: Limpia la pantalla\n",
    "mem: imprime el estado de memoria",
    "ps: imprime el estado de los procesos",
    "kill: recibe un pid y mata al proceso",
    "block: recibe un pid y bloquea al proceso",
    "resume: recibe un pid y resume el proceso",
    "nice: recibe un pid y un valor de prioridad y modifica la prioridad del proceso",
    "test_mm: recibe una cantidad de memoria y empieza a testear",
    "Semaforos: semOpen, semClose, semWait, semPost, printSemInfo",
    "Pipes: pipeOpen, pipeClose, pipeRead, pipeWrite, printPipeInfo"
};

void divZero()
{
  int aux1 = 1;
  int aux2 = 0;
  int o = aux1 / aux2;
}

void invalidOpcode()
{
  _opcodeExp();
}

void printmem(unsigned int argc, char *argv[])
{
  if (argc < 1)
  {
    char *error_message = "No se dio una direccion de memoria\n";
    sys_write(STDERR, error_message, _strlen(error_message));
    return;
  }
  char *address = argv[0];

  // Chequear que strToHex devuelva -1 si no es hex
  uint64_t memDir = strToHex(address);
  if (memDir == -1 || memDir >= LAST_MEM)
  {
    char *error_message = "\nLa direccion ingresada no es alcanzable \n";
    sys_write(STDERR, error_message, _strlen(error_message));
    return;
  }
  _fprintf(STDOUT, "\nDump de 32 bytes a partir de la direccion: %s\n\n", address);

  uint8_t buffer[DUMP_SIZE];
  sys_printMem(memDir, buffer, DUMP_SIZE);
  for (int i = 0; i < DUMP_SIZE; i++)
  {
    if (i % 8 == 0)
    {
      _fprintf(STDOUT, "\n x%d:  ", i);
    }
    _fprintf(STDOUT, "0x%x  ", buffer[i]);
  }
  _fprintf(STDOUT, "\n");
}

void dateAndTime()
{
  uint64_t date = sys_dateAndTime(DAY_RTC_ID);
  uint64_t month = sys_dateAndTime(MONTH_RTC_ID);
  uint64_t year = sys_dateAndTime(YEAR_RTC_ID);
  _fprintf(STDOUT, "La fecha de hoy es: %d/%d/%d", date, month, year);
  uint64_t hour = sys_dateAndTime(HOUR_RTC_ID);
  uint64_t minute = sys_dateAndTime(MINUTE_RTC_ID);
  uint64_t second = sys_dateAndTime(SECOND_RTC_ID);
  _fprintf(STDOUT, "\n Y el horario de este momento es: %d:%d:%d\n", hour, minute, second);
}

void help(unsigned int argc, char *argv[])
{
  if (argc == 1)
  {
    _fprint(STDOUT, "Lista de posibles comandos: \n");
    for (uint8_t i = 0; i < COMMANDS_LENGTH; i++)
    {
      _fprintf(STDOUT, "\t%s\n", commands[i].name);
    }
    _print("Lista de posibles operaciones: \n");
    _print("\tprogram1 | program2\n");
    _print("help [comando]  para desplegar informacion detallada de cada comando\nhelp operaciones para informacion acerca de las operaciones posibles.\n");
  }
  else
  {
    char *description = getDescriptions(argv[1]);
    if (description != NULL)
    {
      _print(description);
    }
    else
    {
      _fprintf(STDOUT, "%s no existe\n", argv[1]);
    }
  }
}

void infoReg()
{
  static uint64_t registers[TOTAL_REGISTERS];
  sys_inforeg(registers);

  for (int i = 0; i < TOTAL_REGISTERS; i++)
  {
    _fprintf(STDOUT, "%s", registerNames[i]);
    _fprintf(STDOUT, "%x\n", registers[i]);
  }
}

void holaMundo()
{
  _print("hola mundo\n");
}

char *getDescriptions(char *function)
{
  for (int i = 0; i < COMMANDS_LENGTH; i++)
  {
    if (!_strcasecmp(function, commands[i].name))
    {
      return commandInfo[i];
    }
  }
  if (!_strcasecmp(function, "operaciones"))
  {
    return commandInfo[COMMANDS_LENGTH];
  }
  return NULL;
}

void clearScreen()
{
  clear_screen(0);
}

command *getCommands()
{
  return commands;
}

void kill(unsigned int argc, char* argv[])
{
  if (argc == 1)
  {
    _fprint(0, "No se ingreso un pid\n");
    return;
  }
  int res = _atoi(argv[1]);
  sys_kill(res);
}

void block(unsigned int argc, char *argv[])
{
  if (argc == 1)
  {
    _fprint(0, "No se ingreso un pid\n");
    return;
  }
  int res = _atoi(argv[1]);
  sys_block(res);
}

void resume(unsigned int argc, char *argv[])
{
  if (argc == 1)
  {
    _fprint(0, "No se ingreso un pid\n");
    return;
  }
  int res = _atoi(argv[1]);
  sys_resume(res);
}

void nice(unsigned int argc, char *argv[])
{
  if (argc == 1)
  {
    _fprint(2, "No se ingreso un pid\n");
    return;
  }
  if (argc == 2)
  {
    _fprint(2, "No se ingreso un valor de nice\n");
  }
  sys_nice(_atoi(argv[1]), _atoi(argv[2]));
}
