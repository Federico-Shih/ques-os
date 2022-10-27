#include <stdlib.h>
#include "string.h"

static int toLower(int chr){
    return (chr >='A' && chr<='Z') ? (chr + 32) : (chr);    
}

int strlen(char * string){
	if(string == NULL)
		return -1;
	int i = 0;
	while(string[i] != '\0')
		i++;
	return i;
}

char * strcpy(char *strDest, const char *strSrc)
{
    char *temp = strDest;
    while( (*(strDest++) = *(strSrc++)) ); 
    return temp;
}

int strcasecmp(const char *str1, const char* str2) {
  while (*str1 != '\0' && *str2 != '\0' && toLower(*str1) == toLower(*str2)) {
    str1 += 1;
    str2 += 1;
  }
  if (*str1 == '\0' && *str2 == '\0') 
  {
    return 0;
  } else if (*str1 == '\0')
  {
    return -1;
  } else if (*str2 == '\0')
  {
    return 1;
  }
  return toLower(*str1) - toLower(*str2);
}