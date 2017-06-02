#ifndef I2C_SETUP_H
#define	I2C_SETUP_H

// Header file for i2c_master_noint.c
// helps implement use I2C1 as a master without using interrupts

void i2c_master_setup(void);              // set up I2C 1 as a master, at 100 kHz
void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop

#define IODIR 0x0
#define GPPU 0x06
#define GPIO 0x09
#define OLAT 0x0A
#define G0 1<<0
#define G1 1<<1
#define G2 1<<2
#define G3 1<<3
#define G4 1<<4
#define G5 1<<5
#define G6 1<<6
#define G7 1<<7  //Define register and pin addresses

#endif	/* I2C_SETUP_H */

