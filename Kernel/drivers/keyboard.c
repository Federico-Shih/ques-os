// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "keyboard.h"
#include "console.h"
#include "naiveConsole.h"
#include "interrupts.h"
#include "syscalls.h"
#include "processManager.h"
#include "pipes.h"
#include "time.h"

// Tablas que mapean scancodes a chars
static char kbd_US[128] =
    {
        0, 0, /* <-- esc */
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', /* <-- Tab */
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, /* <-- control key */
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
        '*',
        0,   /* Alt */
        ' ', /* Space bar */
        0,   /* Caps lock */
        0,   /* 59 - F1 key ... > */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, /* < ... F10 */
        0, /* 69 - Num lock*/
        0, /* Scroll Lock */
        0, /* Home key */
        0, /* Up Arrow */
        0, /* Page Up */
        '-',
        0, /* Left Arrow */
        0,
        0, /* Right Arrow */
        '+',
        0, /* 79 - End key*/
        0, /* Down Arrow */
        0, /* Page Down */
        0, /* Insert Key */
        0, /* Delete Key */
        0, 0, 0,
        0, /* F11 Key */
        0, /* F12 Key */
        0, /* All other keys are undefined */
};

static char shift_kbd_US[128] =
    {
        0, 0, /* <-- esc */
        '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
        '\t', /* <-- Tab */
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
        0, /* <-- control key */
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '`', 0, '|',
        'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
        '*',
        0,   /* Alt */
        ' ', /* Space bar */
        0,   /* Caps lock */
        0,   /* 59 - F1 key ... > */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, /* < ... F10 */
        0, /* 69 - Num lock*/
        0, /* Scroll Lock */
        0, /* Home key */
        0, /* Up Arrow */
        0, /* Page Up */
        '-',
        0, /* Left Arrow */
        0,
        0, /* Right Arrow */
        '+',
        0, /* 79 - End key*/
        0, /* Down Arrow */
        0, /* Page Down */
        0, /* Insert Key */
        0, /* Delete Key */
        0, 0, 0,
        0, /* F11 Key */
        0, /* F12 Key */
        0, /* All other keys are undefined */
};

static int stdin;
static int isSpecial = 0;
static int shiftFlag = 0;
static int controlFlag = 0;
int bufferSemaphore;

void initKeyboardSystem()
{
    stdin = pipeOpen(STDIN_PIPENO);
}

// para que no printee cosas raras cuando toco una tecla no imprimible como el control
int isPrintable(uint8_t teclahex)
{
    return teclahex < RELEASE && kbd_US[teclahex] != 0; // esos son los numeros imprimibles en la tabla ascii
}

void keyboardHandler(uint64_t rsp)
{
    if (!_hasKey())
        return;

    uint8_t teclahex = _getKey();

    if (teclahex == 0xE0)
        isSpecial = 1;

    if (isSpecial)
    {
        teclahex = _getKey();
        isSpecial = 0;
    }

    if (teclahex == 0xE0)
        return;
    if (teclahex == RSHIFT || teclahex == LSHIFT) // si toco shift
        shiftFlag = 1;
    else if (teclahex == RSHIFT + RELEASE || teclahex == LSHIFT + RELEASE) // si suelto shift
        shiftFlag = 0;
    else if (teclahex == LCTRL)
        controlFlag = 1;
    else if (teclahex == LCTRL + RELEASE)
        controlFlag = 0;
    else if (controlFlag)
    {
        switch (kbd_US[teclahex])
        {
        case 'r':
            snapshotRegisters((uint64_t *)rsp);
            break;
        case 'c':
            killCurrentForeground();
            break;
        case 'd':
            writeStdin(-1);
            break;
        }
    }
    else
    {
        if (shiftFlag && isPrintable(teclahex)) // si es algo imprimible (no de retorno)
            writeStdin(shift_kbd_US[teclahex]);
        else if (isPrintable(teclahex))
            writeStdin(kbd_US[teclahex]);
    }
}

void writeStdin(char c)
{
    pipePutchar(STDIN_PIPENO, c);
}

char readStdin()
{
    setCursor(1);
    char input = pipeRead(STDIN_PIPENO);
    setCursor(0);
    return input;
}
