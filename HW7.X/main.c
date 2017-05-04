#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ILI9163C.h"
#include "library.h"
#include "i2c_setup.h"

void drawBar(int, int, unsigned short);

int main(void) {
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
    ANSELBbits.ANSB2 = 0;  // Turn off analog for B2
    ANSELBbits.ANSB3 = 0;  // Turn off analog for B3

    __builtin_enable_interrupts();
    SPI1_init();
    LCD_init();
    LCD_clearScreen(BLACK);
    i2c_master_setup();
    i2c_master_write(CTRL1_XL,0x82);
    i2c_master_write(CTRL2_G,0x88);
    i2c_master_write(CTRL3_C,0x04);
    char message[25], data[14];
    int length = 14;
    signed int xlen, ylen;
    signed short signed_data[7];
    while (1) {
        _CP0_SET_COUNT(0);
        i2c_read_multiple(SLAVE_ADDRESS, OUT_TEMP_L, data, length);
        int i=0,j=0;
        while (i<=length-1){
            signed_data[j]= (data[i+1] << 8) | data[i];
            i=i+2;
            j++;
        }
        xlen=  (55.0*signed_data[4])/16000.0; 
        if (abs(xlen) < 1) {xlen = 0;}
        ylen= (55.0*signed_data[5])/16000.0; 
        if (abs(ylen) < 3) {ylen = 0;}
        drawBar(xlen,ylen, WHITE);
        sprintf(message,"Ax: %6d  Ay: %6d",signed_data[4],signed_data[5]);
        LCD_writeString(7,118,message);
        sprintf(message,"Az: %6d",signed_data[6]);
        LCD_writeString(7,2,message);
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()<2400000) {;}
    }
    return 0;
}

void drawBarChar(unsigned short xpos, unsigned short ypos, unsigned short color, char xsign){
    char xarray[5]= {0x38, 0x38, 0x38, 0x38, 0x38};
    char yarray[5]= {0x00, 0xff, 0xff, 0xff, 0x00};
    int i=0, j=0;
    for (i=0; i<=4; i++) {
        for (j=0; j<=7; j++) {
            if (xsign){
                if (xarray[i]>> j & 1 == 1) {
                    LCD_drawPixel(xpos+i, ypos+j, color);
                }
                else {
                    LCD_drawPixel(xpos+i, ypos+j, BLACK);
                }
            }
            else{
                if (yarray[i]>> j & 1 == 1) {
                    LCD_drawPixel(xpos+i, ypos+j, color);
                }
                else {
                    LCD_drawPixel(xpos+i, ypos+j, BLACK);
                }
            }
        }
    }
}

void drawBar(int xlen, int ylen, unsigned short color) {
    char xstart=63,ystart=63; 
    signed char i=0;
    if (ylen > 0) {
        for (i = 0; i < ylen; i=i+8) drawBarChar(xstart, ystart - i, color,0);
        for (i = ylen+8; i < 100; i=i+8) drawBarChar(xstart, ystart - i, BLACK, 0);
        for (i = 0; i > -100; i=i-8) drawBarChar(xstart, ystart - i, BLACK,0);
    } 
    else {
        for (i = 0; i > ylen; i=i-8) drawBarChar(xstart, ystart - i, color,0);
        for (i = ylen-8; i > -100; i=i-8) drawBarChar(xstart, ystart - i, BLACK,0);
        for (i = 0; i < 100; i=i+8) drawBarChar(xstart, ystart - i, BLACK,0);
    }
    if (xlen > 0) {
        for (i = 0; i < xlen; i=i+5) drawBarChar(xstart-i, ystart,color,1);
        for (i = xlen+5; i < 100; i=i+5) drawBarChar(xstart-i, ystart, BLACK,1);
        for (i = 0; i > -100; i=i-5) drawBarChar(xstart-i, ystart, BLACK,1);
    } 
    else {
        for (i = 0; i > xlen; i=i-5) drawBarChar(xstart-i, ystart, color,1);
        for (i = xlen-5; i > -100; i=i-5) drawBarChar(xstart-i, ystart, BLACK,1);
        for (i = 0; i < 100; i=i+5) drawBarChar(xstart-i, ystart, BLACK,1);
    }
}