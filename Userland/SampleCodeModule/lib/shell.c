// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ctype.h>
#include "ustdlib.h"
#include "shell.h"
#include "usersyscalls.h"
#include "RTCID.h"
#include "commands.h"
#include "colors.h"

#define BUFFER_LENGTH 1024
#define MAX_ARGS 32

static int shellPipeId = 100;
static char buffer[BUFFER_LENGTH];

int getCommandLine(char** args) {
  buffer[BUFFER_LENGTH - 1] = '\0';

  int c;
  int bufferIndex = 0;
  while ((c = getChar()) != '\n' && c != EOF && bufferIndex < BUFFER_LENGTH) {
    switch(c) {
      case '\b':
        if (bufferIndex > 0)
        {
          if (buffer[bufferIndex] == '\t')
          {
            _putc(c);
          }
          buffer[bufferIndex--] = '\0';
          _putc(c);
        }
        break;
      default:
        buffer[bufferIndex++] = c;
        _putc(c);
    }
  }
  if (bufferIndex == 0) return 0;

  // Transformo la linea de args en strings individuales de argumentos
  int isPrevChar = 0;
  int argIndex = 0;
  char *strStart = buffer;
  for (int i = 0; i < bufferIndex; i += 1)
  {
    if (buffer[i] == ' ' && isPrevChar)
    {
      isPrevChar = 0;
      buffer[i] = '\0'; // le doy un fin al string
      args[argIndex] = strStart;
      argIndex += 1;
    }
    // Comienzo de un nuevo string
    else if (!isPrevChar && buffer[i] != ' ')
    {
      strStart = &buffer[i];
      isPrevChar = 1;
    }
  }
  if (isPrevChar)
  {
    buffer[bufferIndex] = '\0';
    args[argIndex] = strStart;
    argIndex += 1;
  }
  return argIndex;
}

int runCommandLine(int argCount, char** args) {
  if (argCount == 0) return 0;
  // Obtiene lista de comandos validos
  
  //primero buscar pipe
  //segundo, si tiene buscar los comandos
  //tercero, correrlos
  int pipeIndex = findPipe(argCount, args);

  if(pipeIndex != -1)
  {
    if (pipeIndex == 0 )
    {
      _fprintf("Pipe no tiene funcion inicial");
      return 0;
    }
    if (pipeIndex == (argCount - 1) )
    {
      _fprintf("Pipe no tiene funcion destino");
      return 0;
    }

    command_t* foundCommand = getCommand(args[0]);
    if (foundCommand == NULL) {
      _fprintf("%s no es un comando valido\n", args[0]);
    return 0;
    }

    foundCommand = getCommand(args[pipeIndex + 1]);
    if (foundCommand == NULL) {
      _fprintf("%s no es un comando valido\n", args[pipeIndex+1]);
    return 0;
    }

    if( linkPipe(argCount, args, pipeIndex) == -1 )
    {
      _fprintf("Error al intentar establecer el pipe");
    }
    return 1;
  }
  command_t* foundCommand = getCommand(args[0]);

  if (foundCommand == NULL) {
    _fprintf("%s no es un comando valido\n", args[0]);
    return 0;
  }

  int foreground = _strcasecmp(args[argCount - 1], "&") != 0;
  // Arma el "llamado" a la funcion, pasandoles los argumentos y la cantidad de argumentos
  unsigned int argc = argCount - (1 - foreground); 
  char **argv = args;
  int pid = sys_startTask(foundCommand->runner, argc, argv, foreground, NULL);
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
  color_t cheeseColor[] = {YELLOW, BLACK};
  clear_screen(1);
  sys_write("Bienvendio a QUESOS\n", 21, cheeseColor);
  help(1, NULL);
  _putc('\n');
  int i = 0;
  while (1) {
    sys_write("QUESOS>", 7, cheeseColor);
    char* args[MAX_ARGS];
    int count = getCommandLine(args);
    _putc('\n');
    runCommandLine(count, args);
    _putc('\n');
    i += 1;
  }
}

void initShell() {
  runShell();
  _fprint("Ended shell\n");
}

int findPipe(int argc, char **argv)
{
  for (int i = 0; i < argc; i++) {
    if (_strcasecmp(argv[i], "|") == 0) {
      return i;
    }
  }
  return -1;
}

// command arg1 arg2 ... argn-1 | command2 argn+2 argn+3 ... argc - 1

int linkPipe(int argc, char **argv, int pipeIndex)
{
  int pids[2];

  int pipe = sys_pipeOpen(shellPipeId++);

  if ( pipe == -1 )
  {
    _fprintf("Error al inicializar el pipe");
    return -1;
  }

  int foreground = _strcasecmp(argv[argc - 1], "&") != 0;

  pids[0] = runPipeCommand(pipeIndex, argv, 0, pipe, foreground);
  if(pids[0] == -1){
    sys_pipeClose(pipe);
    return -1;
  }

  int secondCommandArgc = argc - pipeIndex - 1 - (1 - foreground);
  pids[1] = runPipeCommand(secondCommandArgc, &(argv[pipeIndex + 1]), pipe, 1, foreground);
  if (pids[1] == -1) {
    sys_kill(pids[0]);
    sys_pipeClose(pipe);
    return -1;
  }

  if (foreground)
  {
    waitpid(pids[0]);
    waitpid(pids[1]);
  }
  sys_pipeClose(pipe);
  return 1;
}

int runPipeCommand(int argc, char **argv, int fdin, int fdout, int foreground)
{
  int fd[2];
  fd[0] = fdin;
  fd[1] = fdout;

  command_t* foundCommand = getCommand(argv[0]);

  if (foundCommand == NULL) {
    _fprintf("%s no es un comando valido\n", argv[0]);
    return 0;
  }
  return sys_startTask(foundCommand->runner, argc, argv, foreground, fd);
}