// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ustdlib.h>
#include <usersyscalls.h>
#include <stdint.h>


int _strlen(const char* str) {
  int i;
  for (i = 0; str[i] != '\0'; i += 1);
  return i;
}

void _print(const char *str) {
  _fprint(str);
}

void _fprint(const char* str) {
  unsigned int length = _strlen(str);
  sys_write(str, length, NULL);
}

// Inspirado de https://stackoverflow.com/questions/1735236/how-to-write-my-own-printf-in-c
void _fprintf(char* format, ...) 
{ 
    //Module 1: Initializing Myprintf's arguments 
    va_list arg; 
    va_start(arg, format); 

    char *traverse; 
    unsigned int i; 
    char *s;

    int hasFormat = 0;
     
    for(traverse = format; *traverse != '\0'; traverse++) 
    { 
        if (*traverse == '%') {
          if (hasFormat == 1) {
            _putc('%');
          }
          hasFormat = 1;
        } else if (hasFormat) {
          //Module 2: Fetching and executing arguments
          switch(*traverse) 
          { 
              case 'c' : i = va_arg(arg,int);     //Fetch char argument
                          _putc(i);
                          break; 

              case 'd' : i = va_arg(arg,int);         //Fetch Decimal/Integer argument
                          if(i<0) 
                          { 
                              i = -i;
                              _putc('-'); 
                          } 
                          _fprint(_itoa(i,10));
                          break; 

              case 'o': i = va_arg(arg, int); //Fetch Octal representation
                          _fprint(_itoa(i,8));
                          break; 

              case 's': s = va_arg(arg,char *);       //Fetch string
                          _fprint(s); 
                          break; 

              case 'x': i = va_arg(arg, uint64_t); //Fetch Hexadecimal representation
                          _fprint(_itoa(i,16));
                          break;
              case 'u':    //Fetch uint64_t representation
                          _fprint(_itoa(va_arg(arg, uint64_t), 10));
                          break;
              default:
                          _putc('%');
          }
          hasFormat = 0;
        } else {
          _putc(*traverse);
        }

    } 

    //Module 3: Closing argument list to necessary clean-up
    va_end(arg); 
    return;
} 

void _putc(char c) {
  sys_write(&c, 1, NULL);
}

void clear_screen(unsigned int fd) {
  sys_clear(fd);
}

char getChar() {
  return sys_read();
}

char *_itoa(uint64_t num, int base) 
{ 
    static char Representation[]= "0123456789ABCDEF";
    static char buffer[128]; 
    char *ptr; 

    ptr = &buffer[128]; 
    *ptr = '\0'; 

    do 
    { 
        *--ptr = Representation[num%base]; 
        num /= (uint64_t)base; 
    }while(num != 0); 

    return(ptr); 
}

long _atoi(const char *str) {
  long res = 0;
  int i = 0;

  for (; str[i] == ' '; i += 1); // elimino espacios en blanco
  while (str[i] && IS_DIGIT(str[i])) {
    res = res * 10 + (str[i] - '0');
    i += 1;
  } 
  return res;
}

int isNumber(const char *str)
{
  if (!str) return 0;
  for (int i = 0; str[i] != '\0'; i += 1)
  {
    if (str[i] && !IS_DIGIT(str[i]))
    {
      return 0;
    }
  }
  return 1;
}

int _strcasecmp(const char *str1, const char* str2) {
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

int strToHex(const char *str){
    int val = 0;      
    while( *str != 0 ) {  
        if( !IS_HEXDIGIT(*str) ) { 
            return -1;
        }
        if( IS_DIGIT(*str) ) {    
            val = val * 16 + *str++ - '0';      
        } else if( *str >= 'A' && *str <= 'F' ) {    
            val = val * 16 + *str++ - 'A' + 10;      
        } else {   
            val = val * 16 + *str++ - 'a' + 10;      
        }
    }
    return val;   
}

char * strcpy(char *strDest, const char *strSrc)
{
    char *temp = strDest;
    while( (*(strDest++) = *(strSrc++)) ); 
    return temp;
}


int toLower(int chr){
    return (chr >='A' && chr<='Z') ? (chr + 32) : (chr);    
}