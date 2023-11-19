
#ifndef BINARY_CALCULATOR_HARDWARE_H
#define BINARY_CALCULATOR_HARDWARE_H

extern volatile int *LEDR_ptr;
extern volatile int *SWITCH_ptr;
extern volatile int *HEX_ptr1; 


int open_physical(int fd);
void close_physical(int fd);
void *map_physical(int fd, unsigned int base, unsigned int span);
void init(int fd);
void *readLEDR();
void *readSwitch();
void *readHEX1();
void *readHEX2();
void *readKey();
int decimal_bcd(int decimal);
int writeLEDR(int value);
int writeHEX(int value);
void writeBoth(int value, volatile int *hex_ptr1, volatile int *hex_ptr2);

#endif