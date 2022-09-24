#include <console.h>
#include <colors.h>
#include <naiveConsole.h>
#include <time.h>
#include <string.h>

// Puntero a memoria del dispositivo de video
static uint8_t * const video = (uint8_t*)0xB8000;

// Tamaño total de pantalla
static const uint32_t width = SCREEN_WIDTH;
static const uint32_t height = SCREEN_HEIGHT;

static char buffer[64] = { '0' };

// Definimos los limites de las pantallas, debido a que la consigna indica una pantalla completa y 2 pantallas seaparadas.
static window windows[3] = {
  { //stdout
    SCREEN_WIDTH, SCREEN_HEIGHT, {0, 0}, {0, 0}
  },
  { //stleft
    SCREEN_WIDTH / 2 - 2, SCREEN_HEIGHT, {0, 0}, {0, 0}
  },
  { //stdright
    SCREEN_WIDTH / 2, SCREEN_HEIGHT, {SCREEN_WIDTH / 2 - 1, 0}, {SCREEN_WIDTH / 2 - 1, 0}
  }
};

/*
  ______   __    __  ________   ______           ______    ______  
 /      \ |  \  |  \|        \ /      \         /      \  /      \ 
|  $$$$$$\| $$  | $$| $$$$$$$$|  $$$$$$\       |  $$$$$$\|  $$$$$$\
| $$  | $$| $$  | $$| $$__    | $$___\$$______ | $$  | $$| $$___\$$
| $$  | $$| $$  | $$| $$  \    \$$    \|      \| $$  | $$ \$$    \ 
| $$ _| $$| $$  | $$| $$$$$    _\$$$$$$\\$$$$$$| $$  | $$ _\$$$$$$\
| $$/ \ $$| $$__/ $$| $$_____ |  \__| $$       | $$__/ $$|  \__| $$
 \$$ $$ $$ \$$    $$| $$     \ \$$    $$        \$$    $$ \$$    $$
  \$$$$$$\  \$$$$$$  \$$$$$$$$  \$$$$$$          \$$$$$$   \$$$$$$ 
      \$$$                                                         
                                                                                                           
*/

void printCheeseOs(){
printColor("  ______   __    __  ________   ______           ______    ______  \n", 68, YELLOW, BLACK);
printColor(" /      \\ |  \\  |  \\|        \\ /      \\         /      \\  /      \\ \n", 68, YELLOW, BLACK);
printColor("|  $$$$$$\\| $$  | $$| $$$$$$$$|  $$$$$$\\       |  $$$$$$\\|  $$$$$$\\\n", 68, YELLOW, BLACK);
printColor("| $$  | $$| $$  | $$| $$__    | $$___\\$$______ | $$  | $$| $$___\\$$\n", 68, YELLOW, BLACK);
printColor("| $$  | $$| $$  | $$| $$  \\    \\$$    \\|      \\| $$  | $$ \\$$    \\ \n", 68, YELLOW, BLACK);
printColor("| $$ _| $$| $$  | $$| $$$$$    _\\$$$$$$\\$$$$$$| $$  | $$ _\\$$$$$$\\\n", 67, YELLOW, BLACK);
printColor("| $$/ \\ $$| $$__/ $$| $$_____ |  \\__| $$       | $$__/ $$|  \\__| $$\n", 68, YELLOW, BLACK);
printColor(" \\$$ $$ $$ \\$$    $$| $$     \\ \\$$    $$        \\$$    $$ \\$$    $$\n", 68, YELLOW, BLACK);
printColor("  \\$$$$$$\\  \\$$$$$$  \\$$$$$$$$  \\$$$$$$          \\$$$$$$   \\$$$$$$\n", 67, YELLOW, BLACK);
printColor("      \\$$$\n", 11, YELLOW, BLACK);
newLine();
}

void printCheese(){
  newLine();
  printColor("    _--\"-.           \n", 22, YELLOW, BLACK);
  printColor(" .-\"      \"-.        \n", 22, YELLOW, BLACK);
  printColor("|\"\"--..      \'-.     \n", 22, YELLOW, BLACK);
  printColor("|      \"\"--..   \'-.  \n", 22, YELLOW, BLACK);
  printColor("|.-. .-\".    \"\"--..\".\n", 22, YELLOW, BLACK);
  printColor("|\'./  -\'  .-.      | \n", 22, YELLOW, BLACK);
  printColor("|      .-. \'.-\'   .-\'\n", 22, YELLOW, BLACK);
  printColor("\'--..  \'.\'    .-  -. \n", 22, YELLOW, BLACK);
  printColor("     \"\"--..   \'_\'   :\n", 22, YELLOW, BLACK);
  printColor("           \"\"--..   |\n", 22, YELLOW, BLACK);
  printColor("                 \"\"-\'\n", 22, YELLOW, BLACK);
  newLine();
}


// A que pantalla va a imprimir las funciones de escritura
static int currentWindow = 0;

// Indica si deberia mostrarse el cursor (visual) o no
static int cursorActive = 0;

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

void goNextPosition();
uint8_t* getPosition(int y, int x);

// Devuelve al comienzo el puntero a la posicion de la pantalla actual
void restartCurrentPos() {
  windows[currentWindow].currPos.x = windows[currentWindow].start.x;
  windows[currentWindow].currPos.y = windows[currentWindow].start.y;
}

// Limpia la pantalla
void clearScreen() {
  restartCurrentPos(currentWindow);
    
  for (int h = 0; h <= windows[currentWindow].height; h += 1) {
    newLine(currentWindow);
  }

  restartCurrentPos(currentWindow);
}

// Borra la primera linea y mueve todas las lineas inferiores una posicion hacia arriba
void scrollUp() {
  window win = windows[currentWindow];
  for (int i = 1; i < win.height; i += 1) {
    for (int j = 0; j < win.width; j += 1) {
      *(getPosition(win.start.y + i - 1, win.start.x + j)) = *(getPosition(win.start.y + i, win.start.x + j));
      *(getPosition(win.start.y + i - 1, win.start.x + j) + 1) = *(getPosition(win.start.y + i, win.start.x + j) + 1);
    }
  }

  // Limpia la ultima linea
  for (int j = 0; j < win.width; j += 1) {
    *(getPosition(win.start.y + win.height - 1, j + win.start.x)) = ' ';
    *(getPosition(win.start.y + win.height - 1, j + win.start.x) + 1) = 0;
  }
  windows[currentWindow].currPos.y -= 1;
}

// Imprime en la base numeral que se necesita
void printBase(uint64_t value, uint32_t base) {
  uintToBase(value, buffer, base);
  print(buffer, strlen(buffer));
}

// Imprime un string
void print(char* str, size_t count) {
  for (int i = 0; i < count; i += 1) {
    printChar(str[i]);
  }
}

// Imprime string con color y background color
void printColor(char* str, size_t count, color_t charColor, color_t bgColor) {
  for (int i = 0; i < count; i += 1) {
    printCharColor(str[i], charColor, bgColor, 1);
  }
}

// Imprime una linea hasta el final
void newLine() {
  do {
    printChar(' ');
  } while (windows[currentWindow].currPos.x != windows[currentWindow].start.x);
}

// Imprime caracter en default
void printChar(char c) {
  printCharColor(c, LGREY, BLACK, 1);
}

// Comienza desde cero las pantallas de izquierda, derecha y dibuja una linea que separa las pantallas
void initializeDualScreen() {
  for (int i = 0; i < height; i += 1) {
    *(getPosition(i, width / 2 - 2)) = 219;
  }
  currentWindow = 2;
  restartCurrentPos();
  currentWindow = 1;
  restartCurrentPos();
}

// Comienza desde cero la pantalla principal
void initializeSingleScreen() {
  currentWindow = 0;
  restartCurrentPos();
}

// Borra un caracter
void deleteChar() {
  if(windows[currentWindow].currPos.x == windows[currentWindow].start.x){        
        if (windows[currentWindow].currPos.y > windows[currentWindow].start.y) {
          windows[currentWindow].currPos.y -= 1;                               
          windows[currentWindow].currPos.x = windows[currentWindow].width + windows[currentWindow].start.x -1;
          printCharColor(' ', WHITE, BLACK, 0);
        }
  } else {
    windows[currentWindow].currPos.x = (windows[currentWindow].currPos.x-1);
    printCharColor(' ', WHITE, BLACK, 0);
  }
}

// Escribe un caracter cualquiera con color. Con next decidis si pasar al siguiente caracter o no
void printCharColor(char c, color_t charColor, color_t bgColor, int next){
  if(c == '\b') {
    deleteChar();
    return;
  }

  if (windows[currentWindow].currPos.y == windows[currentWindow].height + windows[currentWindow].start.y) {
    scrollUp();
  }

  // Newline
  if(c == '\n'){
    newLine();
    return;
  }

  if (c == '\t') {
    for (int i = 0; i < 2; i += 1) printCharColor(' ', charColor, bgColor, 1);
    return;
  }
  
  uint8_t* current = getPosition(windows[currentWindow].currPos.y, windows[currentWindow].currPos.x);
  *(current) = c;
  *(current + 1) = ((bgColor << 4) | charColor);
  
  if (next) {
    goNextPosition();
  }
}

// Obtiene la posicion fisica en la memoria a partir de coordenadas y, x (fila, columna)
uint8_t* getPosition(int y, int x) {
  return (video + x * 2 + y * width * 2);
}

// Pasa a la siguiente posicion de la pantalla. Si llega al final de la pantalla, pasa de fila
void goNextPosition() {
  int goNextLine = windows[currentWindow].currPos.x == (windows[currentWindow].start.x + windows[currentWindow].width - 1);
  windows[currentWindow].currPos.x = (!goNextLine) ? windows[currentWindow].currPos.x + 1 : windows[currentWindow].start.x;
  windows[currentWindow].currPos.y += goNextLine;
}

// si vale 1, esta activo
void setCursor(int active) {
  cursorActive = active;
  if (!active) {
    stopCursor();
  }
}

// Imprime el cursor visual dependiendo de cuantos ticks han pasado 
void blinkCursor() {
  if (cursorActive) {
    if(ticks_elapsed() % 18 == 0)
        printCharColor(' ', WHITE, BLACK, 0);
    else if(ticks_elapsed() % 9 == 0)
        printCharColor('_', WHITE, BLACK, 0);
  }
}

// Borra el cursor
void stopCursor() {
  printCharColor(' ', WHITE, BLACK, 0);
}

// Cambia de pantalla
void switchScreens(size_t screen) {
  if (screen < 3) {
    currentWindow = screen;
  }
} 

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}