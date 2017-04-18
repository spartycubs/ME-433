#include <stdio.h>
#include <stdlib.h>
#include "library.h"
#include "i2c_setup.h"
#include "pin_exp.h"
// I2C Homework
int main(void) {
    init_expander();
    while (1) {
        if (((get_expander()>>7) && 1) == 1) {
            set_expander(G0,1);
        }
        else {
            set_expander(G0,0);
        }
    }
    return 0;
}

