#pragma once
#include "io.h"
#include "typedefs.h"

#define VGA_MEMORY (char*)0xB8000
#define VGA_WIDTH 80
#define MAX_ROWS 25


struct 
{
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t returnRawPosition(){
        return (y * 80) + x;
    }
} cursorPosition;


void SetCursorPosition(uint16_t position);
void clear_screen(void);
void print_char(char s);
void printf(const char* _text);
void printInteger(int _data);
char* _ttyPrintInteger(int _integer);
void resetCursorPosition(void);
void _printf(const char* fmt, ...); //Unsecure function
