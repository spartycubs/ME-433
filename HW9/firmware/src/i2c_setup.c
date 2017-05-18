// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be called in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k
#include "i2c_setup.h"
#include <xc.h>


void i2c_master_write(char reg_address, char byte) {
    i2c_master_start();                       // make the start bit
    i2c_master_send(SLAVE_ADDRESS << 1 | 0);  // write the address, shifted left by 1
                                              //    or'd with a 0 because we're writing
    i2c_master_send(reg_address);             // the register to write to
    i2c_master_send(byte);                    // the value to put in the register
    i2c_master_stop();                        // make the stop bit
}

char i2c_master_read(char reg_address) {
    i2c_master_start();                       // make the start bit
    i2c_master_send(SLAVE_ADDRESS << 1 | 0);  // write the address, shifted left by 1
                                              //    or'd with a 0 because we're writing
    i2c_master_send(reg_address);             // the register to read from
    i2c_master_restart();                     // make the restart bit
    i2c_master_send(SLAVE_ADDRESS << 1 | 1);  // write the address, shifted left by 1
                                              //    or'd with a 1 because we're reading
    unsigned char r = i2c_master_recv();      // save the value returned
    i2c_master_ack(1);                        // make the ack so the slave knows we received
    i2c_master_stop();                        // make the stop bit
    return r;
}

void i2c_read_multiple(unsigned char address, unsigned char reg, unsigned char * data, int length) {
    int i;
    i2c_master_start();                       // make the start bit
    i2c_master_send(address << 1 | 0);        // write the address, shifted left by 1
                                              //    or'd with a 0 because we're writing
    i2c_master_send(reg);                     // the register to read from
    i2c_master_restart();                     // make the restart bit
    i2c_master_send(address << 1 | 1);        // write the address, shifted left by 1
                                              //    or'd with a 1 because we're reading
    for (i=0;i<length;i++) {
        data[i] = i2c_master_recv();          // save the value returned
        if (i == length-1) {
            i2c_master_ack(1);
        }
        else {
            i2c_master_ack(0);                    // make the ack so the slave knows we want to continue reading
        }
    }
    i2c_master_stop();                        // make the stop bit
}

void i2c_master_setup(void) {
    I2C2BRG = 233;            // I2CBRG = [[1/(2*100000) - 104*10^-9]*48000000] - 2 
    I2C2CONbits.ON = 1;       // turn on the I2C1 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
    I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
    while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
    if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
      // ("I2C2 Master: failed to receive ACK\r\n");
    }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
    I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
    while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}