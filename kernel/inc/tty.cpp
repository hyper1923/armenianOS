#include "tty.h"
#include "../mem/memory.h"
#include "../timer/timer.h"

void SetCursorPosition(uint16_t position){
    outb(0x3D4,0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    outb(0x3D4, 0x0e);
    outb(0x3D5,(uint8_t)((position >> 8) & 0xFF));
}

uint16_t poscords(uint16_t x, uint16_t y){
    return y * VGA_WIDTH + x;
}

void clear_screen(void){
    uint32_t maxSize = MAX_ROWS * VGA_WIDTH;
    uint8_t* VGA_MEM = (uint8_t*)VGA_MEMORY;
    for(int i = 0; i < maxSize; i++){
        *(VGA_MEM + i * 2) = 0; 
    }
    cursorPosition.x = 0;
    cursorPosition.y = 0;
    SetCursorPosition(cursorPosition.returnRawPosition());
}

void clear_col(int col){
    uint8_t* VGA_MEM = (uint8_t*)VGA_MEMORY + (160 * col);
    for(int i = 0; i < 80; i++){
        *(VGA_MEM + i * 2) = 0;  
        *(VGA_MEM + i * 2+1) = 7;  
    }
}

void resetCursorPosition(void){
    cursorPosition.x = 0;
    cursorPosition.y = 0;
}

void print_char(char s, uint16_t color)
{
    if(cursorPosition.returnRawPosition() >= 2000){
        memcut((void*)0xB8000, (void*)(0xB8000 + (80 * 2)), 3840);
        cursorPosition.x = 0;
        cursorPosition.y = 24;
        SetCursorPosition(cursorPosition.returnRawPosition());
        clear_col(24);
    } else{
        *(VGA_MEMORY + cursorPosition.returnRawPosition() *2) = s;
        *(VGA_MEMORY + cursorPosition.returnRawPosition() *2 + 1) = color;
        cursorPosition.x++; 
        SetCursorPosition(cursorPosition.returnRawPosition());
    }
}

void printf(const char* _text){
    uint8_t* data = (uint8_t*)_text; 
    while(*data != 0){
        switch (*data)
        {
            case '\n':
                cursorPosition.x = 0;
                cursorPosition.y++;
                SetCursorPosition(cursorPosition.returnRawPosition());
                data++;
                break;
            default:
                print_char(*data,VGA_WHITEGRAY);
                data++;  
                break;
        }
    }
}


void _printf(const char* fmt, ...){
    int* argp = (int*)&fmt;
    argp += sizeof(fmt) / sizeof(int);
    uint8_t* ignored;
    uint8_t* data = (uint8_t*)fmt;
    while(*data != 0){
         switch (*data)
        {
            case '\n':
                cursorPosition.x = 0;
                cursorPosition.y++;
                SetCursorPosition(cursorPosition.returnRawPosition());
                data++;
                break;
            case '\t':
                cursorPosition.x += 4;
                SetCursorPosition(cursorPosition.returnRawPosition());
                data++;
                break;
            case '%':
                switch (*(data + 1))
                {
                    case 'd':
                        ignored = data +1;
                        printf(_ttyPrintInteger(*argp));
                        argp++;
                        break;
                    case 's': {
                        ignored = data+1;
                        printf(*(char**)argp);
                        argp++;
                        break;
                    }
                    case 'c': {
                        ignored = data+1;
                        print_char(*(char*)argp,VGA_WHITEGRAY);
                        argp++;
                        break;
                    }
                    default:
                        print_char('%',VGA_WHITEGRAY);
                        break;
                }
            default:
                if(ignored != data && ((*data != '%' && *(data + 1) != 'd')|| (*data != '%' && *(data + 1) != 's') || (*data != '%' && *(data + 1) != 'c')))   
                    print_char(*data,VGA_WHITEGRAY);
                data++;  
                break;
        }
    }
}
char* _ttyPrintInteger(int _integer){
    int i;
    char* txt; 
    i = 0;
    do{
        txt[i++] = _integer % 10 + '0';
    } while((_integer /= 10) > 0);
    txt[i] = '\0';

    int lenght = (strlen(txt)-1);
    uint32_t a, b;
    uint8_t temp;
    for(a = 0,b = lenght; a < b; a++, b--){
        temp = txt[a];
        txt[a] = txt[b];
        txt[b] = temp;
    }

    return txt;
}
void printInteger(int _data){  
        int rem;
        int ones;
        int tens;
        int hundreds;
        int thousands;
        int tensThousands;

        int size;
        int sizeTester = (int)_data;
        while(sizeTester / 10 > 0){
            sizeTester /= 10;
            size++;
        }
        for (int i = 0; i < 5; i++) 
        {
            rem = _data %10;
            _data = _data /10;

            if(i == 0)
                ones = rem;
            else if(i == 1)
                tens = rem;
            else if(i == 2)
                hundreds = rem;
            else if(i == 3)
                thousands = rem;
            else if(i == 4)
                tensThousands = rem;
        }

        for (int i = 0; i < 6; i++)
        {
            switch (i)
            {
            case 0:
                print_char('0' + tensThousands,VGA_WHITEGRAY);
                break;
            case 1:
                print_char('0' + thousands,VGA_WHITEGRAY);
                break;
            case 2:
                print_char('0' + hundreds,VGA_WHITEGRAY);
                break;
            case 3:
                print_char('0' + tens,VGA_WHITEGRAY);
                break;
            case 4:
                print_char('0' + ones,VGA_WHITEGRAY);
                break;                
            }
        }
        
    }
void _printCurrentEntries(void){
    _printf("%d", cursorPosition.returnRawPosition());
}