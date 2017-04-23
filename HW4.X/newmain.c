#include <stdio.h>
#include <stdlib.h>
#include "library.h"
#include <math.h>

#define CS LATBbits.LATB15              // chip select pin

static volatile unsigned char sine_wave[200];
static volatile unsigned char tri_wave[200];   // Define vectors for waveforms

void init_spi1() {
    // set up the chip select pin as an output
    TRISBbits.TRISB15 = 0;
    CS = 1;
        
    // SS1Rbits.SS1R = 0b0011;  // Set B15 as the SS1 input pin
    SDI1Rbits.SDI1R = 0b0100;   // Set B8 as the SDI1 input pin
    RPB13Rbits.RPB13R = 0b0011; // Set B13 as the SDO1 output pin
    
    SPI1CON = 0;
    SPI1BUF;                    // Clear buffer by reading from it
    SPI1BRG = 0x1000;             // Baud rate is 12 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;    // clear the overflow bit
    //SPI1CONbits.MODE32 = 0;
    //SPI1CONbits.MODE16 = 0;     // Use 8 bit mode
    SPI1CONbits.CKE = 1;
    SPI1CONbits.MSTEN = 1;      // master operation
    SPI1CONbits.SSEN = 0;
    SPI1CONbits.ON = 1;         // Turn on SPI1
}

void waveforms(void) {
    int i;
    for (i=0; i<200; i++) {
        float temp;
        temp = 127.5 + 127.5* sin(2 * 3.14159*10* (i /200.0));
        sine_wave[i] =  (unsigned char) temp;
        temp = 255.0 * i/200.0;                     // 5 Hz triangle wave
        tri_wave[i] = (unsigned char) temp;
    }
}

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
    SPI1BUF = o;
    while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
        ;
    }
    return SPI1BUF;
}

void setVoltage (char channel, unsigned char volt) {
    CS = 0;
    spi_io((channel << 15) | 0x7000 | (volt >> 4));  // Setting SPI1BUF from MCP4902 data sheet
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT()<2400){;}
    spi_io(volt << 4);
    CS = 1;
}

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

    __builtin_enable_interrupts();
    
    init_spi1();
    waveforms();
    while(1) {
        static int counter = 0;
        if (_CP0_GET_COUNT() >= 24000) {        // Cycle 1000x per second
            setVoltage(0,sine_wave[counter]);
            setVoltage(1,tri_wave[counter]);    // Set voltages for each Vout
            if (counter == 199) {
                counter = 0;                    // Reset wave counters
            }
            counter++;
        }
    }
    return 0;
}