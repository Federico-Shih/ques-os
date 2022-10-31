#ifndef STDIO_H
#define STDIO_H
#include <stdint.h>
#include "colors.h"

int printf(char const *fmt, ...);
char *reverse(char *buffer, int i, int j);
void swap(char *x, char *y);
char *intToStr(int value, char *buffer, int base);
int sysWrite(const char *buffer, uint64_t size, color_t *colors);
char sysRead();
#endif