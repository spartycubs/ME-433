#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "library.h"
#include "ILI9163C.h"

void LCD_writeLetter(int, int, char);
void bar_init();

int main(void) {
    char message[25];
    __builtin_disable_interrupts();
    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;
    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;
    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;

    __builtin_enable_interrupts();
    SPI1_init();
    LCD_init();
    
    int i = 0, j, num, x_pos = 28, y_pos = 32;

    LCD_clearScreen(BLACK);
    bar_init();
    _CP0_SET_COUNT(0);
    for (num = 0;num < 101;num++) {
        if (_CP0_GET_COUNT() >= 4800000) {        // Cycle 5x per second
            _CP0_SET_COUNT(0);
            sprintf(message, "Hello World %d!",num);
            for (j=0;j<5;j++) {
                LCD_drawPixel(14+num,63+j,WHITE);  // Draw progress step on bar
            }
            while (message[i]) {
                LCD_writeLetter(x_pos,y_pos,message[i]);  // Write string to screen
                i++;
                x_pos += 5;
            }
        }
    }

    //LCD_writeLetter(x_pos,y_pos,'h');
    return 0;
}

void LCD_writeLetter (int x, int y, char letter) {
    int i, j;
    for (i=0;i<5;i++) {
        for (j=0;j<8;j++){
            if ((ASCII[letter-0x20][i] >> j) & 0x01 == 1) {
                LCD_drawPixel(x+i,y+j,WHITE);
            }
            else {
                LCD_drawPixel(x+i,y+j,BLACK);
            }
        }
    }
}

void bar_init () {
    int j;
    for (j=0;j<102;j++) {
        LCD_drawPixel(14+j,62,WHITE);
        LCD_drawPixel(14+j,68,WHITE);
    }
    for (j=0;j<7;j++) {
        LCD_drawPixel(14,62+j,WHITE);
        LCD_drawPixel(115,62+j,WHITE);
    }
}