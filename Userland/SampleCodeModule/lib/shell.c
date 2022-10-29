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
          _putc(c);
        }
        break;
      case ' ':
        buffer[i++] = '\0';
        strings[res_index++] = startString;
        startString = buffer + i;
        _putc(c);
        break;
      default:
        buffer[i++] = c;
        _putc(c);
    }
  }
  if (i == 0) return 0;
  buffer[i] = '\0'; //Resuelve el ultimo string presente
  strings[res_index++] = startString;
  return res_index;
}

int runCommandLine(int argCount, char** args) {
  if (argCount == 0) return 0;
  // Obtiene lista de comandos validos
  
  command_t* foundCommand = getCommand(args[0]);

  if (foundCommand == NULL) {
    _fprintf("%s no es un comando valido\n", args[0]);
    return 0;
  }

  int foreground = _strcasecmp(args[argCount - 1], "&") != 0;
  // Arma el "llamado" a la funcion, pasandoles los argumentos y la cantidad de argumentos
  caller command;
  command.argCount = argCount - (1 - foreground); 
  command.args = args;
  command.runner = foundCommand->runner;
  int pid = sys_startTask(command.runner, command.argCount, command.args, foreground, NULL);
  if (pid == -1)
  {
    _fprintf( "ERROR STARTING TASK\n");
    return 0;
  }
  if (foreground)
  {
    waitpid(pid);
  }
  return 1;
}

void runShell() {
  clear_screen(1);
  help(1, NULL);
  _putc('\n');
  while (1) {
    // sys_showCursor(1);
    _print("QUESOS>");
    char* args[MAX_ARGS];
    int count = getCommandLine(args);
    _putc('\n');
    // sys_showCursor(0);
    runCommandLine(count, args);
    _putc('\n');
  }
}

void initShell() {
  runShell();
  while(1);
}

