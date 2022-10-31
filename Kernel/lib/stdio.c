#include <stdarg.h>
#include "stdio.h"
#include "processManager.h"
#include "console.h"
#include "pipes.h"
#include "keyboard.h"
#include "string.h"

static int scan(char const *fmt, va_list arg);

int printf(char const *fmt, ...) {
  va_list arguments;
  int length;

  va_start(arguments, fmt);
  length = scan(fmt, arguments);
  va_end(arguments);
  return length;
}

// https://www.techiedelight.com/implement-itoa-function-in-c/

int abs(int num) { return num < 0 ? -num : num; }

// function to reverse buffer[i..j]
char *reverse(char *buffer, int i, int j) {
  while (i < j) swap(&buffer[i++], &buffer[j--]);

  return buffer;
}

void swap(char *x, char *y) {
  char t = *x;
  *x = *y;
  *y = t;
}

// Iterative function to implement itoa() function in C
char *intToStr(int value, char *buffer, int base) {
  // invalid input
  if (base < 2 || base > 32) {
    return buffer;
  }

  // consider the absolute value of the number
  int n = abs(value);

  int i = 0;
  while (n) {
    int r = n % base;

    if (r >= 10) {
      buffer[i++] = 65 + (r - 10);
    } else {
      buffer[i++] = 48 + r;
    }

    n = n / base;
  }

  // if the number is 0
  if (i == 0) {
    buffer[i++] = '0';
  }

  // If the base is 10 and the value is negative, the resulting string
  // is preceded with a minus sign (-)
  // With any other base, value is always considered unsigned
  if (value < 0 && base == 10) {
    buffer[i++] = '-';
  }

  buffer[i] = '\0';  // null terminate string

  // reverse the string and return it
  return reverse(buffer, 0, i - 1);
}

static int scan(char const *fmt, va_list arg) {
  int int_temp;
  char char_temp;
  char *string_temp;

  char ch;
  int length = 0;

  char buffer[512];

  while ((ch = *fmt++)) {
    if ('%' == ch) {
      switch (ch = *fmt++) {
        case '%':
          printChar('%');
          length++;
          break;
        case 'c':
          char_temp = va_arg(arg, int);
          printChar(char_temp);
          length++;
          break;
        case 's':
          string_temp = va_arg(arg, char *);
          printline(string_temp);
          length += strlen(string_temp);
          break;
        case 'd':
          int_temp = va_arg(arg, int);
          intToStr(int_temp, buffer, 10);
          printline(buffer);
          length += strlen(buffer);
          break;
        case 'x':
          int_temp = va_arg(arg, int);
          intToStr(int_temp, buffer, 16);
          print("0x", 2);
          printline(buffer);
          length += strlen(buffer);
          break;
      }
    } else {
      printChar(ch);
      length++;
    }
  }
  return length;
}

int sysWrite(const char *buffer, uint64_t size, color_t *colors)
{
    if (buffer == 0 || size == 0)
        return -1;

    pcb *currentProcess = getCallingProcess();
    int i;

    color_t textColor = (colors) ? colors[0] : LGREY;
    color_t bgColor = (colors) ? colors[1] : BLACK;

    for (i = 0; i < size && buffer[i]; i += 1)
    {
        if (currentProcess->fileDescriptors[1] == STDOUT_PIPENO)
            printCharColor(buffer[i], textColor, bgColor, 1);
        else
        {
            int written = pipePutchar(currentProcess->fileDescriptors[1], buffer[i]);
            if (written == -1)
              return -1;
        }
    }
    return i;
}


char sysRead()
{
    pcb *current = getCallingProcess();
    int readPipe = current->fileDescriptors[0];
    if (readPipe == 0)
    {
        if (current->foreground)
            return readStdin();
        return -1;
    }
    else
        return pipeRead(readPipe);
}