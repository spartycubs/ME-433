#include <stdio.h>
#include <stdlib.h>
#include "library.h"
#include "i2c_setup.h"
#include "pin_exp.h"
// I2C Homework
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

    __builtin_enable_interrupts();
    
    init_expander();
    while (1) {
        if ((get_expander()>>7) == 0) {
            set_expander(G0,1);
        }
        else {
            set_expander(G0,0);
        }
    }
    return 0;
}

