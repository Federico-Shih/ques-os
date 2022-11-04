#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>
#include <stdlib.h>

// para obtener un background se hace << 4
// para utilizar un color con un backround
// hay que hacer ((BACKCOLOR << 4) | FOREGROUNDCOLOR)
#define BLACK    0x00
#define BLUE     0x01
#define GREEN    0x02
#define CYAN     0x03
#define RED      0x04
#define MAGENTA  0x05
#define BROWN    0x06
#define LGREY    0x07
#define DGREY    0x08
#define LBLUE    0x09
#define LGREEN   0x0A
#define LCYAN    0x0B
#define LRED     0x0C
#define LMAGENTA 0x0D
#define YELLOW   0x0E
#define WHITE    0x0F
#define DEFAULTCOLOR (LGREY+(BLACK<<4))

typedef uint8_t color_t;

#endif //COLORS_H