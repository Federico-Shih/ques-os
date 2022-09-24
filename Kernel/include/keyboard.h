#ifndef KEYBOARD_DRIVER
#define KEYBOARD_DRIVER

#include <stdint.h>
#include <console.h>
#include <lib.h>
#include <colors.h>

#define MAXCHARACTERS 25*80
#define RELEASE 128 //el offset entre el codigo de una tecla y el codigo del release de esa tecla
#define BACKSPACE 14
#define ENTER 28
#define LSHIFT 42
#define RSHIFT 54
#define LCTRL 29
//#define RCTRL //averiguar que onda
#define ENDKEY 79
#define BUFFER_LENGTH 256

extern uint8_t _getKey(); //en libasm.asm
extern uint8_t _hasKey();

typedef struct bufferStruct{
    int write_i, read_i;
    int overflow; // necesario para chequear que read_i sea menor que write_i ciclicamente
    uint16_t buffer[BUFFER_LENGTH];
}bufferStruct;

typedef enum {CHARBUFFER, SCANCODEBUFFER} bufferIndex;

//busca el numero de caracter en mi array y lo imprime
void printKey(uint8_t key);

void printKeyMayus(uint8_t key);

void keyboardHandler(uint64_t rsp);
void loadInBuffer(uint8_t c);
int getFromBuffer();
void cleanBuffer();
int bufferSize();
int removeFromBuffer();
void toggleBuffer(bufferIndex buf);


#endif