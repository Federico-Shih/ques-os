#ifndef STRING_H
#define STRING_H

#include <stdlib.h>

int strlen(char * string);
char * strcpy(char *strDest, const char *strSrc);
int strcasecmp(const char *str1, const char* str2);
char *strncpy(char *strDest, const char *strSrc, size_t n);

#endif