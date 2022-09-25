#include <ustdlib.h>
#include <shell.h>
#include <ctype.h>
#include <usersyscalls.h>
#include <RTCID.h>
#include <commands.h>

#define BUFFER_LENGTH 256
#define MAX_ARGS 32

int getCommandLine(char** strings) {
  static char buffer[BUFFER_LENGTH];
  buffer[BUFFER_LENGTH - 1] = '\0'; // Por las dudas

  char* startString = buffer; // Guarda el puntero al string que esta obteniendo
  int c;
  int i = 0;
  int res_index = 0;
  while ((c = getChar()) != '\n' && i < BUFFER_LENGTH - 1 && res_index < MAX_ARGS) {
    switch(c) {
      case '\b':
        if (i != 0) {
          i--;
          _putc(STDOUT, c);
        }
        break;
      case ' ':
        buffer[i++] = '\0';
        strings[res_index++] = startString;
        startString = buffer + i;
        _putc(STDOUT, c);
        break;
      default:
        buffer[i++] = c;
        _putc(STDOUT, c);
    }
  }
  if (i == 0) return 0;
  buffer[i] = '\0'; //Resuelve el ultimo string presente
  strings[res_index++] = startString;
  return res_index;
}

int runCommandLine(int argCount, char** args) {
  // Obtiene lista de comandos validos
  command* commandList = getCommands();

  int firstCommandIndex = -1;
  char* firstCommand = args[0]; // hola <- | mundo

  // Busca el supuesto commando en la lista de commandos
  for (int i = 0; i < COMMANDS_LENGTH && (firstCommandIndex == -1); i += 1) {
    if (_strcasecmp(firstCommand, commandList[i].name)) {
      firstCommandIndex = i;
    }
  }

  if (firstCommandIndex == -1) {
    _fprintf(STDOUT, "%s no es un comando valido\n", firstCommand);
    return 0;
  }

  // Arma el "llamado" a la funcion, pasandoles los argumentos y la cantidad de argumentos
  caller callers[1];
  callers[0].argCount = (argCount - 1); 
  callers[0].args = (void**)&(args[1]);
  callers[0].runner = commandList[firstCommandIndex].runner;
  callers[0].screenId = 0;

  callers->runner(callers->argCount, callers->args);
  return 1;
}

void runShell() {
  clear_screen(1);
  help(0, NULL);
  _putc(STDOUT, '\n');

  while (1) {
    sys_showCursor(1);
    _print("QUESOS>");
    char* args[MAX_ARGS];
    int count = getCommandLine(args);
    _putc(STDOUT, '\n');
    sys_showCursor(0);
    runCommandLine(count, args);
    _putc(STDOUT, '\n');
  }
}

void initShell() {
  runShell();
  while(1);
}

