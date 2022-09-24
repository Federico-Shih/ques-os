#include <ustdlib.h>
#include <shell.h>
#include <ctype.h>
#include <usersyscalls.h>
#include <RTCID.h>
#include <commands.h>

#define BUFFER_LENGTH 256
#define MAX_ARGS 32

/*
  P para pausar y resumir
  ESC para terminar
*/

void startPausableProgram(unsigned int count, void** args) {
  caller* c = (caller*)args[0];
  uint64_t childPid = sys_child(c);

  bool paused = 0;
  while(sys_hasChild()) {
    int key = getKey();
    if (key == P) {
      if (paused) {
        sys_resume(childPid);
      } else {
        sys_pause(childPid);
      }
      paused = !paused;
    } else if (key == ESCAPE) {
      break;
    }
  }
  sys_exit();
}

/*
  A para pausar y resumir left
  D para pausar y resumir right
  ESC para terminar
*/

void startPipe(unsigned int count, void** args) {
  caller* leftCaller = (caller*)args[0];
  caller* rightCaller = (caller*)args[1];

  clear_screen(0);
  clear_screen(1);
  clear_screen(2);
  sys_toggleMode(1);
  uint64_t leftPid = sys_child(leftCaller);
  uint64_t rightPid = sys_child(rightCaller);
  bool pausedLeft = 0, pausedRight = 0;

  while (1) {
    int key = getKey();
    if (key == ESCAPE) {
      break;
    } else if (key == A) {
      if (pausedLeft) {
        sys_resume(leftPid);
      } else {
        sys_pause(leftPid);
      }
      pausedLeft = !pausedLeft;
    } else if (key == D) {
      if (pausedRight) {
        sys_resume(rightPid);
      } else {
        sys_pause(rightPid);
      }
      pausedRight = !pausedRight;
    }
  }
  clear_screen(0);
  sys_toggleMode(0);
  sys_exit();
}

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
  if (argCount == 0 || args == NULL) return 0;
  int pipeIndex = -1;

  // Busca el pipe
  for (int i = 0; i < argCount && (pipeIndex == -1); i += 1) {
    if (_strcasecmp(args[i], "|")) {
      pipeIndex = i;
    }
  }

  // Obtiene lista de comandos validos
  command* commandList = getCommands();

  if (pipeIndex == 0) {
    _print("El operador pipe no tiene primer operando\n");
    return 0;
  }
  if (pipeIndex == (argCount - 1)) {
    _print("El operador pipe no tiene segundo operando\n");
    return 0;
  }

  int firstCommandIndex = -1;
  int secondCommandIndex = -1;
  char* firstCommand = args[0]; // hola <- | mundo
  char* secondCommand = args[pipeIndex + 1]; // hola | mundo <-

  // Busca el supuesto commando en la lista de commandos
  for (int i = 0; i < COMMANDS_LENGTH && (firstCommandIndex == -1 || secondCommandIndex == -1); i += 1) {
    if (_strcasecmp(firstCommand, commandList[i].name)) {
      firstCommandIndex = i;
    }
    if (_strcasecmp(secondCommand, commandList[i].name)) {
      secondCommandIndex = i;
    }
  }

  if (firstCommandIndex == -1) {
    _fprintf(STDOUT, "%s no es un comando valido\n", firstCommand);
    return 0;
  }
  if (secondCommandIndex == -1) {
    _fprintf(STDOUT, "%s no es un comando valido\n", secondCommand);
    return 0;
  }

  // Arma el "llamado" a la funcion, pasandoles los argumentos y la cantidad de argumentos
  caller callers[2];
  callers[0].argCount = (pipeIndex != -1) ? (pipeIndex - 1) : (argCount - 1); 
  callers[0].args = (void**)&(args[1]);
  callers[0].runner = commandList[firstCommandIndex].runner;
  callers[0].screenId = (pipeIndex == -1) ? 0 : 1;

  callers[1].argCount = argCount - pipeIndex - 2; 
  callers[1].args = &(args[pipeIndex + 2]);
  callers[1].runner = (void**)commandList[secondCommandIndex].runner; // Vale lo mismo que firstCommandIndex si se llama sin pipe
  callers[1].screenId = 2;

  if (pipeIndex == -1) {
    void* args[1] = { callers };

    caller pausableCaller = { &startPausableProgram, args, 1, 0 };
    sys_start(&pausableCaller);
  } else {
    void* args[2] = { &(callers[0]), &(callers[1]) };
    caller pipeCaller = { &startPipe, args, 2, 0 };
    sys_start(&pipeCaller);
  }

  return 1;
}

void runShell(unsigned int count, void** args) {
  clear_screen(1);
  caller helpCall = { &help, NULL, 0, 0 };
  sys_start(&helpCall);
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
  caller c = { &runShell, NULL, 0, 0 };
  sys_start(&c);
  while(1);
}

