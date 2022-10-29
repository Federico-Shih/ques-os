#include <math.h>
#include "commands.h"
#include "ustdlib.h"
#include "stdint.h"
#include "RTCID.h"
#include "usersyscalls.h"
#include "fibonacci.h"
#include "prime.h"
#include "inforeg.h"
#include "tests.h"
#include "phylo.h"
#include "colors.h"

static color_t ERROR_COLORS[] = {RED, BLACK};

static command_t commands[] = {
    {"date&time", &dateAndTime, "date&time : Imprime en patalla la fecha del ano corriente y horario en que fue llamado."},
    {"divZero", &divZero, "divzero: Realiza una division por 0. Lanza una excepcion e imprime una captura de los registros al momento de ejecucion. "},
    {
        "fibonacci",
        &fibonacci,
        "fibonacci: Imprime la secuencia de fibonacci infinitamente hasta que se pause o se termine su ejecucion. ",
    },
    {
        "hello",
        &holaMundo,
        "hello: Imprime un saludo al usuario. ",
    },
    {
        "help",
        &help,
        "help: Imprime una lista detallada de los comandos  y modulos ejecutables del programa. ",
    },
    {"inforeg", &infoReg, "inforeg: Imprime los registros registros capturados al presionar ctrl + r. "},
    {
        "invalidOpcode",
        &invalidOpcode,
        "invalidOpcode: Lanza la excepcion de invalid operand code e imprime los registros al momento de ejecucion. ",
    },
    {
        "prime",
        &primes,
        "prime: imprime numeros primos infinitamente hasta que se pause o se termine su ejecucion. ",
    },
    {"printmem", &printmem, "printmem [DIRECCION DE MEMORIA]: Recibe como argumento una direccion de memoria no superior a 80000000h y luego imprime los proximos 32bytes de memoria adyacentes a la direccion dada. "},
    {"clear", &clearScreen, "clear: Limpia la pantalla "},
    {
        "mem",
        &sys_memDump,
        "mem: imprime el estado de memoria",
    },
    {
        "ps",
        &sys_printProcesses,
        "ps: imprime el estado de los procesos",
    },
    {"kill", &kill, "kill: recibe un pid y mata al proceso"},
    {"block", &block, "block: recibe un pid y bloquea al proceso"},
    {"resume", &resume, "resume: recibe un pid y resume el proceso"},
    {"nice", &nice, "nice: recibe un pid y un valor de prioridad y modifica la prioridad del proceso"},
    {"loop", &loop, "loop: te saluda cada cierto tiempo"},
    {"sem", &sys_printSemInfo, "sem: imprime informacion sobre los semaforos"},
    {"pipes", &sys_printPipeInfo, "pipes: imprime estado de los pipes"},
    {"test_mm", &test_mm, "test_mm: recibe una cantidad de memoria y empieza a testear"},
    {"test_prio", &test_prio, "test_prio: testea las prioridades del scheduler"},
    {"test_processes", &test_processes, "test_processes: testea el scheduler"},
    {"test_sync", &test_sync, "test_sync: testea semaforos y race conditions"},
    {"phylo", &phylo, "phylo: filosofos comensales"},
    {"cat", &cat, "cat: imprime el stdin tal como lo recibe"},
    {"wc", &wc, "wc: cuenta la cantidad de lineas escritas. Escriba stop para terminar y recibir el numero"},
    {"filter", &filter, "filter: filtra las vocales del input"},
    {"", NULL, ""},
};

int getCommandsLength()
{
  int i;
  for (i = 0; commands[i].runner != NULL; i += 1)
    ;
  return i;
}

void divZero()
{
  int aux1 = 1;
  int aux2 = 0;
  int o = aux1 / aux2;
  _fprintf("%d\n", o);
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
    sys_write(error_message, _strlen(error_message), ERROR_COLORS);
    return;
  }
  char *address = argv[0];

  // Chequear que strToHex devuelva -1 si no es hex
  uint64_t memDir = strToHex(address);
  if (memDir == -1 || memDir >= LAST_MEM)
  {
    char *error_message = "\nLa direccion ingresada no es alcanzable \n";
    sys_write(error_message, _strlen(error_message), ERROR_COLORS);
    return;
  }
  _fprintf("\nDump de 32 bytes a partir de la direccion: %s\n\n", address);

  uint8_t buffer[DUMP_SIZE];
  sys_printMem(memDir, buffer, DUMP_SIZE);
  for (int i = 0; i < DUMP_SIZE; i++)
  {
    if (i % 8 == 0)
    {
      _fprintf("\n x%d:  ", i);
    }
    _fprintf("0x%x  ", buffer[i]);
  }
  _fprintf("\n");
}

void dateAndTime()
{
  uint64_t date = sys_dateAndTime(DAY_RTC_ID);
  uint64_t month = sys_dateAndTime(MONTH_RTC_ID);
  uint64_t year = sys_dateAndTime(YEAR_RTC_ID);
  _fprintf("La fecha de hoy es: %d/%d/%d", date, month, year);
  uint64_t hour = sys_dateAndTime(HOUR_RTC_ID);
  uint64_t minute = sys_dateAndTime(MINUTE_RTC_ID);
  uint64_t second = sys_dateAndTime(SECOND_RTC_ID);
  _fprintf("\n Y el horario de este momento es: %d:%d:%d\n", hour, minute, second);
}

void help(unsigned int argc, char *argv[])
{
  long page = 0;

  if (argc > 1)
  {
    char *description = getDescriptions(argv[1]);
    if (description != NULL)
    {
      _print(description);
      return;
    }
    else
    {
      if (isNumber(argv[1]))
      {
        page = _atoi(argv[1]);
      }
      else
      {
        _fprintf("%s no existe\n", argv[1]);
        return;
      }
    }
  }

  int length = getCommandsLength();
  if (page > length / HELP_PAGE)
  {
    _fprintf( "No existe esta pagina de comandos: \n");
    return;
  }
  _fprintf( "Lista de posibles comandos: \n");
  for (uint8_t i = 0; commands[i + page * HELP_PAGE].runner != NULL && i < HELP_PAGE; i++)
  {
    _fprintf("%s\n", commands[i + page * HELP_PAGE].help);
  }
  _fprintf("Pagina nro %d de %d, help [pagina] para mas comandos.\n", page, length / HELP_PAGE);
}

void infoReg()
{
  static uint64_t registers[TOTAL_REGISTERS];
  sys_inforeg(registers);

  for (int i = 0; i < TOTAL_REGISTERS; i++)
  {
    _fprintf("%s", registerNames[i]);
    _fprintf("%x\n", registers[i]);
  }
}

void holaMundo()
{
  _print("hola mundo\n");
}

char *getDescriptions(char *function)
{
  command_t *foundCommand = getCommand(function);
  if (foundCommand == NULL)
    return NULL;
  return foundCommand->help;
}

void clearScreen()
{
  clear_screen(0);
}

command_t *getCommand(char *commandName)
{
  for (uint8_t i = 0; _strcasecmp(commands[i].name, "") != 0; i++)
  {
    if (_strcasecmp(commands[i].name, commandName) == 0)
      return &commands[i];
  }

  return NULL;
}

command_t *getCommands()
{
  return commands;
}

void kill(unsigned int argc, char *argv[])
{
  if (argc == 1)
  {
    _fprintf( "No se ingreso un pid\n");
    return;
  }
  int res = _atoi(argv[1]);
  sys_kill(res);
}

void block(unsigned int argc, char *argv[])
{
  if (argc == 1)
  {
    _fprintf( "No se ingreso un pid\n");
    return;
  }
  int res = _atoi(argv[1]);
  sys_block(res);
}

void resume(unsigned int argc, char *argv[])
{
  if (argc == 1)
  {
    _fprintf( "No se ingreso un pid\n");
    return;
  }
  int res = _atoi(argv[1]);
  sys_resume(res);
}

void nice(unsigned int argc, char *argv[])
{
  if (argc == 1)
  {
    _fprintf( "No se ingreso un pid\n");
    return;
  }
  if (argc == 2)
  {
    _fprintf( "No se ingreso un valor de nice\n");
  }
  sys_nice(_atoi(argv[1]), _atoi(argv[2]));
}

void cat(unsigned int argc, char *argv[])
{
  if (argc != 1)
  {
    _fprintf("Uso incorrecto de comando");
    return;
  }
  int c;
  while ( (c = getChar()) )
    _putc(c);
}

void wc(unsigned int argc, char *argv[])
{
  if (argc != 1)
  {
    _fprintf("Uso incorrecto de comando");
    return;
  }
  int c, counter = 1, word = 0;
  int flag1 = 0, flag2 = 0, flag3 = 0;
  while ( (c = getChar()) )
  {
    if ( c == '\n' )
      counter++;
    if ( c == 's' || flag1 )
    {
      flag1 = 1;
      if(c == 's')
        word++;
      if ( (c == 't' && flag1) || flag2 )
      {
        flag2 = 1;
        if(c == 't')
          word++;
        if ( (c == 'o' && flag2) || flag3 )
        {
          flag3 = 1;
          if(c == 'o')
            word++;
          if ( c== 'p' && flag3 )
          {
            if(c == 'p')
              word++;
            if(word == 4){
              _putc(c);
              _fprintf("\nLa cantidad de lineas escritas fue %d", counter);
              return;
            }else{
              word = 0;
            }
          }
        }
      }
    }
    _putc(c);
  }
  return;
}

void filter(unsigned int argc, char *argv[])
{
  if (argc != 1)
  {
    _fprintf("Uso incorrecto de comando");
    return;
  }
  int c;
  while ( (c = getChar() ))
  {
    if( c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u'){
      _fprintf("\n - Letra filtrada! Sus movimientos estan siendo monitoreados - \n");
    }
    else{
      _putc(c);
    }
  }
}

void loop()
{
  while (1)
  {
    _fprintf("Hola soy %d\n", sys_getpid());
    sys_wait(3);
  }
}