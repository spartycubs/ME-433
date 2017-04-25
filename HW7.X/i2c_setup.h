#ifndef I2C_SETUP_H
#define	I2C_SETUP_H

// Header file for i2c_master_noint.c
// helps implement use I2C2 as a master without using interrupts

#define SLAVE_ADDRESS 0b1101011
#define WHOAMI 0x0F
#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12
#define OUT_TEMP_L 0x20
#define OUTX_X_XL 0x28

char i2c_master_read(char address);
void i2c_master_write(char address, char byte);
void i2c_read_multiple(unsigned char address, unsigned char reg, unsigned char * data, int length);
void i2c_master_setup(void);              // set up I2C 1 as a master, at 100 kHz
void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop

#endif	/* I2C_SETUP_H */

