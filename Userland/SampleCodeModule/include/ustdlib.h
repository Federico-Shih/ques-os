#ifndef USTDLIB_H
#define USTDLIB_H

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

enum {
  STDIN = 0, STDOUT, STDERR, STDLEFT, STDRIGHT,
};

#define IS_DIGIT(x) (((x) >= '0' && (x) <= '9'))
#define IS_HEXDIGIT(x) (IS_DIGIT(x) || (x >= 'A' && x <= 'F') || (x >= 'a' && x <= 'f'))

void clear_screen(unsigned int fd);
void _fprint(uint8_t fd, const char *str);
int getChar();
void _putc(uint8_t fd, char c);
void _fprintf(uint8_t fd, char *,...);              
int toLower(int chr);
int strToHex(const char *str);
char* _itoa(uint64_t, int);      
long _atoi(const char* s);
void _print(const char *str);
int _strcasecmp(const char *str1, const char* str2);
int getKey();
int _strlen(const char* str);

void _hlt();

#endif