#include <xc.h>
#include <sys/attribs.h>
#include <stdio.h>
#include <stdlib.h>
#include "i2c_setup.h"

// I2C Homework

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

#define SLAVE_ADDRESS 0x20

void i2c_master_write(char reg_address, char byte) {
    i2c_master_start();                       // make the start bit
    i2c_master_send(SLAVE_ADDRESS << 1 | 0);  // write the address, shifted left by 1
                                              //    or'd with a 0 because we're writing
    i2c_master_send(reg_address);             // the register to write to
    i2c_master_send(byte);                    // the value to put in the register
    i2c_master_stop();                        // make the stop bit
}

char i2c_master_read(char reg_address) {
    unsigned char r = 0;
    i2c_master_start();                       // make the start bit
    i2c_master_send(SLAVE_ADDRESS << 1 | 0);  // write the address, shifted left by 1
                                              //    or'd with a 0 because we're writing
    i2c_master_send(reg_address);             // the register to read from
    i2c_master_restart();                     // make the restart bit
    i2c_master_send(SLAVE_ADDRESS << 1 | 1);  // write the address, shifted left by 1
                                              //    or'd with a 1 because we're reading
    r = i2c_master_recv();      // save the value returned
    i2c_master_ack(1);                        // make the ack so the slave knows we received
    i2c_master_stop();                        // make the stop bit
    return r;
}

void init_expander() {
    i2c_master_setup();
    i2c_master_start();
    i2c_master_send(SLAVE_ADDRESS<<1 | 0);
    i2c_master_send(IODIR);
    i2c_master_send(0xF0);
    i2c_master_stop(); 
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDRESS<<1 | 0);
    i2c_master_send(GPPU);
    i2c_master_send(0xF0);
    i2c_master_stop(); 

    i2c_master_start();
    i2c_master_send(SLAVE_ADDRESS<<1 | 0);
    i2c_master_send(GPIO);
    i2c_master_send(0x00);
    i2c_master_stop(); 
}

void set_expander(char pin, char level) {
    unsigned char write = level << pin;
    i2c_master_start();
    i2c_master_send(SLAVE_ADDRESS<<1 | 0);
    i2c_master_send(GPIO);
    i2c_master_send(write);
    i2c_master_stop();
}

char get_expander() {
    unsigned char r=0;
    i2c_master_start();
    i2c_master_send(SLAVE_ADDRESS<<1 | 0);
    i2c_master_send(GPIO);
    i2c_master_restart();
    i2c_master_send((SLAVE_ADDRESS<<1) | 1);
    r=i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return r;
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
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    ANSELBbits.ANSB2=0;
    ANSELBbits.ANSB3=0;

    __builtin_enable_interrupts();
    
    init_expander();
    
    while (1) {
        if (((get_expander()>>7) & 1) == 1) {set_expander(G0,1);}
        else {set_expander(G0,0);}
    }
}

