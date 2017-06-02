#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xc.h>
#include <sys/attribs.h>

// DEVCFG0
#pragma config DEBUG = 0b10 // no debugging
#pragma config JTAGEN = 0b0 // no jtag
#pragma config ICESEL = 0b11 // use PGED1 and PGEC1
#pragma config PWP = 0x3f//0b111111111 // no write protect
#pragma config BWP = 0b0 // no boot write protect
#pragma config CP = 0 // no code protect

// DEVCFG1
#pragma config FNOSC = 0b011 // use primary oscillator with pll
#pragma config FSOSCEN = 0b0 // turn off secondary oscillator
#pragma config IESO = 0b0 // no switching clocks
#pragma config POSCMOD = 0b10 // high speed crystal mode
#pragma config OSCIOFNC = 0b1// free up secondary osc pins
#pragma config FPBDIV = 0b00 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = 0b10 // do not enable clock switch
#pragma config WDTPS = 0b10100 // slowest wdt
#pragma config WINDIS = 0b1 // no wdt window
#pragma config FWDTEN = 0b0 // wdt off by default
#pragma config FWDTWINSZ = 0b11 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = 0b001 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = 0b111 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = 0b001 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = 0b001 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = 0b0 // USB clock on

// DEVCFG3
#pragma config USERID = 0b0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0b0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0b0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 0b1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 0b1 // USB BUSON controlled by USB module


#define CS LATBbits.LATB15              // chip select pin

static volatile unsigned char sine_wave[100];
static volatile unsigned char tri_wave[100];   // Define vectors for waveforms
double temp;

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
    SPI1BUF = o;
    while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
        ;
    }
    return SPI1BUF;
}

void init_spi1() {
    
        
    // SS1Rbits.SS1R = 0b0011;  // Set B15 as the SS1 input pin
    SDI1Rbits.SDI1R = 0b0100;   // Set B8 as the SDI1 input pin
    RPB13Rbits.RPB13R = 0b0011; // Set B13 as the SDO1 output pin
    // set up the chip select pin as an output
    TRISBbits.TRISB15 = 0;
    CS = 1;
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
    int i=0;
    for (i=0; i<100; i++){
        temp= 2*i*(255.0/100.0);
        tri_wave[i]= (unsigned char) temp;
    }
    i=0;
    for (i=0; i<100; i++) {
        temp=255.0/2.0 + 255.0/2.0* sin((2 * 3.14) * i /100.0);
        sine_wave[i] =  (unsigned char) temp;
    }
}


void setVoltage (unsigned char channel, unsigned char volt) {
    unsigned char val1=0, val2=0, valval=0;
    valval= (unsigned char) volt >> 4;
    if (channel == 1) {
        val1= (unsigned char) (0b11110000 | valval); 
    }
    else {
        val1= (unsigned char) (0b01110000 | valval);
    }
    val2 = (unsigned char) volt << 4;
    CS=0;
    spi_io(val1);
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT()<2400){;}
    spi_io(val2);
    CS=1;
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
    static int counter = 0;
    while(1) {
        setVoltage(0,sine_wave[counter]);
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT()<24000){;}
        setVoltage(1,tri_wave[counter]);    // Set voltages for each Vout
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT()<24000){;}
        if (counter == 99) {
            counter = 0;                    // Reset wave counters
        }
        counter++;
    }
}