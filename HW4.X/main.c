#include <stdio.h>
#include <stdlib.h>
#include "library.h"
#include <math.h>

#define CS LATBbits.LATB8              // chip select pin

static volatile float sine_wave[200];
static volatile float tri_wave[200];   // Define vectors for waveforms

void waveforms(void) {
    static int i;
    for (i=0; i<200; i++) {
        sine_wave[i] = 1.65+1.65*sin(2*3.14159*10*(0.1*i/200));  // 10 Hz sine wave
        tri_wave[i] = (3.3/0.2)*(0.2*i/200);                     // 5 Hz triangle wave
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

void setVoltage (char channel, float voltage) {
    int convert = voltage;                              // Make float voltage into int convert
    CS = 0;
    spi_io((channel << 15) | 0x7000 | (convert << 4));  // Setting SPI1BUF from MCP4902 data sheet
    CS = 1;
}

void init_spi1() {
    // set up the chip select pin as an output
    TRISBbits.TRISB8 = 0;
    CS = 1;
        
    SS1Rbits.SS1R = 0b0011;     // Set B15 as the SS1 input pin
    SDI1Rbits.SDI1R = 0b0100;   // Set B8 as the SDI1 input pin
    RPB13Rbits.RPB13R = 0b0011; // Set B13 as the SDO1 output pin
    
    SPI1BUF;                   // Clear buffer by reading from it
    SPI1BRG = 0x1;             // Baud rate is 12 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;   // clear the overflow bit
    SPI1CONbits.MODE32 = 0;
    SPI1CONbits.MODE16 = 0;    // Use 8 bit mode
    SPI1CONbits.MSTEN = 1;     // master operation
    SPI1CONbits.ON = 1;        // Turn on SPI1
    
}

int main(void) {
    init_spi1();
    waveforms();
    while(1) {
        static int counter = 0;
        if (_CP0_GET_COUNT() >= 24000) {        // Cycle 1000x per second
            _CP0_SET_COUNT(0);                  // Reset count
            setVoltage(0,sine_wave[counter]);
            setVoltage(1,tri_wave[counter]);    // Set voltages for each Vout
            if (counter == 199) {
                counter = 0;                    // Reset wave counters
            }
        }
    }
    return 0;
}