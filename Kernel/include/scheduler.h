#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

/*
  NOTPRESENT: La task no existe
  RUNNING: La task esta corriendo
  PAUSED: La task esta pausada
  NOTINITIALIZED: La task no llamo la funcion a correr
*/
typedef enum status {
  NOTPRESENT, RUNNING, PAUSED, NOTINITIALIZED
} status;


/*
caller: struct que encapsula una funcion a correr
    runner: funcion a correr con argumentos genericos
    args y argCount: argumentos con que deberia ser corrido la funcion
    screenId: pantalla a la cual deberia imprimir la funcion
*/
typedef struct caller {
  int (*runner)(unsigned int count, void** args);
  void** args;
  unsigned int argCount;
  unsigned int screenId;
} caller;

/*
task: unidad de informacion para el scheduler. Representa un estado de funcion 
    program: funcion a correr
    rsp: detalle mas abajo
    parentId: id de la task que genero la task actual
    status: el estado actual de la task
    id: identificador de la task

El rsp que recibe la task es bastante especial. Antes de llamar al timer tick, 
pusheamos todos los registros de la funcion que estaba corriendo. Si queremos recuperar
esa funcion, simplemente popeamos los registros y hacemos iretq. Si tuviesemos otra funcion,
con otro rsp que han pusheado los registros de esa funcion, si cambiamos el rsp y popeamos,
cuando haga iretq volveria a la funcion nueva.

Al principio el rsp es 0, porque la funcion no armo el stack frame. Cuando se llama la funcion,
se mueve el rsp a un lugar especifico del stack, con 1024 posiciones de stack para poder almacenar
la funcion. Cuando tenga que "guardarse" esa funcion, simplemente actualizamos el rsp de la task,
que va a tener "almacenado" todo lo que necesita para volver a la funcion, solo necesitando popstate
e iretq para cambiar de funcion.
*/
typedef struct task {
  caller program;
  uint64_t rsp;
  uint64_t parentId;
  status status;
  uint64_t id;
} task;

// Cambia la task actual. Si forced = 1, no guarda el rsp que recibe
uint64_t switchTask(uint64_t rsp, bool forced);

// Crea una task en la lista de tasks, pausa la task actual y llama la funcion
void startTask(caller* function, uint64_t rsp);

// Crea una task en la lista de tasks y devuelve el id de la task
uint64_t startChild(caller* function);

// Mata la task, poniendolo en status=NOTPRESENT. Todos las tasks hijas que dependan de kill
bool killTask(uint64_t pid);

// Pausa la task
bool pauseTask(uint64_t pid);

// Resume la Task
int resumeTask(uint64_t pid);

// Obtiene el program id de la task actual
uint64_t getPid();

// Devuelve si la task actual tiene hijos que dependen de el
bool currentHasChilds();


#endif