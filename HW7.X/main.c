#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ILI9163C.h"
#include "library.h"
#include "i2c_setup.h"

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
        sprintf(message,"Ax: %4d  Ay: %4d",signed_data[4],signed_data[5]);
        LCD_writeString(14,118,message);
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()<2400000) {;}
    }
    return 0;
}

