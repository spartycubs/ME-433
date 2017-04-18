#ifndef PIN_EXP_H
#define	PIN_EXP_H

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

// Function Prototypes
void init_expander();
void set_expander(char pin, char level);
char get_expander();

#endif	/* PIN_EXP_H */

