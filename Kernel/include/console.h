#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <stdlib.h>
#include <colors.h>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

typedef struct {
	uint8_t x;
	uint8_t y;
} vec;

typedef struct {
    uint8_t width;
    uint8_t height;
    vec start;
    vec currPos;
} window;


typedef enum {
  STDIN = 0, STDOUT, STDERR
} FILE_DESCRIPTOR;


void loadWindows();
void newLine();
void printBase(uint64_t value, uint32_t base);
void printCharColor(char c, color_t charColor, color_t bgColor, int next);
void printChar(char c);
void clearScreen();
void scrollUp();
void print(char* str, size_t count);
void printColor(char* str, size_t count, color_t charColor, color_t bgColor);
void initializeDualScreen();
void blinkCursor();
void stopCursor();
void setCursor(int active);
void switchScreens(size_t screen);
void initializeSingleScreen();
void initializeDualScreen();
void printCheese();
void printCheeseOs();

#endif

