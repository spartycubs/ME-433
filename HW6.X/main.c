#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "library.h"
#include "ILI9163C.h"

void LCD_writeLetter(int, int, char);
void bar_init(int, int, int);

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
    
    int i = 0, j, x_pos = 28, y_pos = 32;
    float time = 0;

    LCD_clearScreen(BLACK);
    bar_init(14, 62, 101);
    _CP0_SET_COUNT(0);
    while (1) {
        if (_CP0_GET_COUNT() >= 4800000) {        // Cycle 5x per second
            static int counter = 0;
            sprintf(message, "Hello World %d!",counter);
            for (j=0;j<5;j++) {
                LCD_drawPixel(14+counter,63+j,WHITE);  // Draw progress step on bar
            }
            LCD_writeString(x_pos,y_pos,message);
            time =(float) 24000000.0/_CP0_GET_COUNT();
            sprintf(message,"Frames: %5.3f",time);
            LCD_writeString(14,75,message);
            i = 0;
            if (counter < 100) {counter++;}
            else {
                counter = 0;
                LCD_clearScreen(BLACK);
                bar_init(14, 62, 101);
            }
            _CP0_SET_COUNT(0);
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

LCD_writeString (int x, int y, char* array) {
    int i = 0;
    while (array[i]) {
        LCD_writeLetter(x + 5*i,y,array[i]);  // Write string to screen
        i++;
    }
}

void bar_init (int x, int y, int length) {
    int j;
    for (j=0;j<length;j++) {
        LCD_drawPixel(x+j,y,WHITE);
        LCD_drawPixel(x+j,y+6,WHITE);
    }
    for (j=0;j<7;j++) {
        LCD_drawPixel(x,y+j,WHITE);
        LCD_drawPixel(x+length,y+j,WHITE);
    }
}