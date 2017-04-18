#include <stdio.h>
#include <xc.h>
#include "pin_exp.h"
#include "i2c_setup.h"

void init_expander() {
    ANSELBbits.ANSB2 = 0;  // Turn off analog for B2
    ANSELBbits.ANSB3 = 0;  // Turn off analog for B3
    i2c_master_setup();
    i2c_master_write(IODIR,0xF0);    
    i2c_master_write(GPPU,0xF0);
    i2c_master_write(GPIO,0x00);
}

void set_expander(char pin, char level) {
    unsigned char write = level << pin;
    i2c_master_write(GPIO,write);
}

char get_expander() {
    return i2c_master_read(GPIO);
}