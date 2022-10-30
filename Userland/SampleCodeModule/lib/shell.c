#include <ustdlib.h>
#include <shell.h>
#include <ctype.h>
#include <usersyscalls.h>
#include <RTCID.h>
#include <commands.h>

#define BUFFER_LENGTH 256
#define MAX_ARGS 32

static int shellPipeId = 100;

int getCommandLine(char** strings) {
  static char buffer[BUFFER_LENGTH];
  buffer[BUFFER_LENGTH - 1] = '\0'; // Por las dudas

  char* startString = buffer; // Guarda el puntero al string que esta obteniendo
  int c;
  int i = 0;
  int res_index = 0;
  while ((c = getChar()) != '\n' && c != -1 && i < BUFFER_LENGTH - 1 && res_index < MAX_ARGS) {
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

int findPipe(int argc, char **argv)
{
  for (int i = 0; i < argc; i++) {
    if (_strcasecmp(argv[i], "|") == 0) {
      return i;
    }
  }
  return -1;
}

int linkPipe(int argc, char **argv, int pipeIndex)
{
  char *arguments[MAX_ARGS];
  int argCount;
  int pids[2];

  int pipe = sys_pipeOpen(shellPipeId++);

  if ( pipe == -1 )
  {
    _fprintf("Error al inicializar el pipe");
    return -1;
  }

  // en 0 hasta pipe esta el primero comando
  for (int i = 0; i < pipeIndex; i++) {
    arguments[i] = argv[i];
    argCount++;
  }

  pids[0] = runPipeCommand(argCount, arguments, 0, pipe);
  if(pids[0] == -1){
    sys_pipeClose(pipe);
    return -1;
  }

  // de pipe hasta el final esta el segundo comando
  for (int i = pipeIndex + 1, j = 0; i < argc; i++, j++) {
    arguments[j] = argv[i];
    argCount++;
  }

  pids[1] = runPipeCommand(argCount, arguments, pipe, 1);
  if (pids[1] == -1) {
    sys_pipeClose(pipe);
    return -1;
  }


  // esta bien????
  waitpid(pids[1]);
  waitpid(pids[0]);

  sys_pipeClose(pipe);

  return 1;
}

int runPipeCommand(int argc, char **argv, int fdin, int fdout)
{
  int fd[2];
  fd[0] = fdin;
  fd[1] = fdout;

  command_t* foundCommand = getCommand(argv[0]);

  if (foundCommand == NULL) {
    _fprintf("%s no es un comando valido\n", argv[0]);
    return 0;
  }

  int foreground = _strcasecmp(argv[argc - 1], "&") != 0;
  // Arma el "llamado" a la funcion, pasandoles los argumentos y la cantidad de argumentos
  caller command;
  command.argCount = argc - (1 - foreground); 
  command.args = argv;
  command.runner = foundCommand->runner;

  return sys_startTask(command.runner, command.argCount, command.args, foreground, fd);
}