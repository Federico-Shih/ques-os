// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pipes.h"
#include "sem.h"
#include <stdint.h>
#include "queue.h"
#include <stdlib.h>
#include "stdio.h"

queueADT pipeQueue = NULL;
int allPipesSem; //para evitar condiciones de carrera en creacion, destruccion e impresion de pipes

typedef struct t_pipe {
  int id;
  char buffer[PIPE_BUFFER_SIZE];
  int writeIndex, readIndex; //porque el array es circular
  int totalProcesses; //para saber si se debe borrar un pipe al hacer un close
  int writeSemId, readSemId;
  //la funcion de write y readSemId es utilizar de manera correcta el buffer circular
} t_pipe;


//funciones auxiliares
static t_pipe *createPipe(int id);
static void writeToPipe(t_pipe * pipe, char c);
static int findPipeCondition(void * queueElement, void * value);
static t_pipe * getPipe(int pipeId);

// devuelve != 0 si funciono, 0 si hubo error
int initPipeSystem(){
    if(pipeQueue == NULL){
        pipeQueue = initQueue();
        allPipesSem = semInit(1);
    }
    return pipeQueue != NULL;
}

//se fija si el pipe existe
//si existe le suma un proceso a totalProcesses
//si no existe lo crea y le suma el proceso
int pipeOpen(int pipeId) {
  t_pipe * pipe;
  semWait(allPipesSem);
  if ((pipe = getPipe(pipeId)) == NULL){
    pipe = createPipe(pipeId);
    if (pipe == NULL)
        return -1;
  }
  pipe->totalProcesses++;
  semPost(allPipesSem);
  return pipe->id;
}

//si el pipe no existe tira error
//escribe en el pipe hasta el NULL terminated
//devuelve -1 con error, 0 sin error
int pipeWrite(int pipeId, char *str) {
    t_pipe * pipe;
    if((pipe = getPipe(pipeId)) == NULL)
        return -1;

    for (int i = 0; str[i] != 0; i++)
    {
        writeToPipe(pipe, str[i]);
    }

    return 0;
}

// TODO: Para aquellas escrituras que son sucesivas, considerar cachear getPipe.
int pipePutchar(int pipeId, char c){
    t_pipe * pipe;
    if((pipe = getPipe(pipeId)) == NULL)
        return -1;

    writeToPipe(pipe, c);
    return 0;
}

//devuelve el char leido si funciono, -1 si hubo error
int pipeRead(int pipeId) {
    t_pipe * pipe;
    if((pipe = getPipe(pipeId)) == NULL)
        return -1;

    char c;

    semWait(pipe->readSemId);

    c = pipe->buffer[pipe->readIndex];
    // circular buffer
    pipe->readIndex = (pipe->readIndex + 1) % PIPE_BUFFER_SIZE;

    semPost(pipe->writeSemId);

    return c;
}

//si el pipe no existe tira error
//le resta uno a totalProcesses. si esta variable es menor a cero destruye el pipe
int pipeClose(int pipeId) {
    t_pipe * pipe;
    if((pipe = getPipe(pipeId)) == NULL)
        return -1;

    semWait(allPipesSem);
    pipe->totalProcesses--;
    
    if (pipe->totalProcesses <= 0){
        semClose(pipe->readSemId);
        semClose(pipe->writeSemId);
        removeElement(pipeQueue, findPipeCondition, (void *)&pipeId); //todo fijarse si falta algun free
    }
    semPost(allPipesSem);
    return 0;
}

void printPipeInfo(){
    semWait(allPipesSem);
    printf("Active Pipe Status\n");
    iteratorADT it = toBegin(pipeQueue);
    t_pipe * pipe = NULL;
    while(hasNext(it)){
        pipe = (t_pipe *)next(it);
        printf("Pipe ID: %d\n", pipe->id);
        printf("ID semaforo de lectura: %d\n", pipe->readSemId);
        printf("ID semaforo de escritura: %d\n", pipe->writeSemId);
        printf("Procesos con el semaforo abierto: %d\n", pipe->totalProcesses);
        printf("Lista de procesos bloqueados:\n");
        printf("\tBloqueados para escribir: ");
        printBlockedPids(pipe->writeSemId);
        printf("\b\b\n\tBloqueados para leer: ");
        printBlockedPids(pipe->readSemId);
        printf("\b\b\nCantidad de caracteres disponibles para leer: %d", ((pipe->writeIndex - pipe->readIndex)%PIPE_BUFFER_SIZE));

    }
    free(it);
    semPost(allPipesSem);
}

// ------------ FUNCIONES AUXILIARES -----------------

static void writeToPipe(t_pipe * pipe, char c){
    semWait(pipe->writeSemId);
    pipe->buffer[pipe->writeIndex] = c;
    pipe->writeIndex = (pipe->writeIndex + 1) % PIPE_BUFFER_SIZE;
    semPost(pipe->readSemId);
}

static t_pipe *createPipe(int id) {
  t_pipe * pipe = malloc(sizeof(t_pipe));
  if (pipe == NULL) return NULL;

  pipe->id = id;
  pipe->readIndex = pipe->writeIndex = pipe->totalProcesses = 0;

  pipe->readSemId = semInit(0);
  pipe->writeSemId = semInit(PIPE_BUFFER_SIZE);
  if(pipe->readSemId == -1 || pipe->writeSemId == -1)
    return NULL;

  enqueue(pipeQueue, pipe);
  return pipe;
}

// -------------- para funciones de la cola de pipes -----------------

static int findPipeCondition(void * queueElement, void * value){
  return (((t_pipe*)queueElement)->id == (*(int*)value));
}

static t_pipe * getPipe(int pipeId){
    return find(pipeQueue, findPipeCondition, &pipeId);
}

// -------------------------------------------------------------------