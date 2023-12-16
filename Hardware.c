#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include "./address_map_arm.h"

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

// Global variables: Pointers to Light Emitting Diodes (LEDs), Switches, and 7 segment displays.
// These are volatile to indicate their values can change at any time due to external hardware changes.
volatile int *LEDR_ptr;
volatile int *SWITCH_ptr;
volatile int *HEX_ptr1; 
void *LW_virtual;
void *virtual_base;


int open_physical(int fd);
void close_physical(int fd);
void* map_physical(int fd, unsigned int base, unsigned int span);

// Declaration of functions for opening and closing physical memory, and mapping physical memory to virtual memory space.
void init(int fd){
    if ((fd = open_physical (fd)) == -1)
		return;
	if ((LW_virtual = map_physical (fd, LW_BRIDGE_BASE, LW_BRIDGE_SPAN)) == NULL)
		return;
	if((virtual_base = map_physical (fd, HW_REGS_SPAN, HW_REGS_SPAN)) == NULL)
		return;
}

void *getLW_Virtual(){
	return LW_virtual;
}
void *getVirtual_base(){
	return virtual_base;
}

// Functions for obtaining virtual addresses for LED, switch, and HEX display components by adding base addresses to LW_virtual.
void *readLEDR(){
    return (unsigned int *)(LW_virtual + LEDR_BASE);   
}
void *readSwitch(){
    return (unsigned int *)(LW_virtual + SW_BASE);   
}
void *readHEX1(){
    return (unsigned int *)(LW_virtual + HEX3_HEX0_BASE);   
}
void *readHEX2(){
    return (unsigned int *)(LW_virtual + HEX5_HEX4_BASE);
}
void *readKey(){
	return (unsigned int*)(LW_virtual + KEY_BASE);
}

// Opens "/dev/mem" for read/write access to physical memory, returning a file descriptor.
// If opening fails, an error message is printed and -1 is returned.
int open_physical (int fd) {
   if (fd == -1)
      if ((fd = open( "/dev/mem", (O_RDWR | O_SYNC))) == -1)
      {
         printf ("ERROR: could not open \"/dev/mem\"...\n");
         return (-1);
      }
   return fd;
}

// Closes the file descriptor for physical memory access.
void close_physical (int fd) {
   close (fd);
}

// Maps a range of physical addresses to virtual addresses for access from user space.
// On failure, it prints an error, closes the file descriptor, and returns NULL.
void* map_physical(int fd, unsigned int base, unsigned int span) {
   void *virtual_base;

   // Get a mapping from physical addresses to virtual addresses
   virtual_base = mmap (NULL, span, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, base);
   if (virtual_base == MAP_FAILED)
   {
      printf ("ERROR: mmap() failed...\n");
      close (fd);
      return (NULL);
   }
   return virtual_base;
}

// Converts a given integer value into a corresponding LED pattern using switch-case logic.
int writeLEDR(int value){
	switch(value){
		case 0:
			 return 1;
		break;
		case 1:
			return 2;
		break;
		case 2:
			return 4;
		break;
		case 3:
			return 8;
		break;
		case 4:
			return 16;
		break;
		case 5:
			return 32;
		break;
		case 6:
			return 64;
		break;
		case 7:
			return 128;
		break;
		default:
			return 0;
		break;
	}
}

// Converts a decimal number to its Binary Coded Decimal (BCD) representation for HEX display.
int decimal_bcd(int decimal) {
	switch (decimal)
	{
		case 0:
			return 0x3f;
		case 1:
			return 0x06;
		case 2:
			return 0x5b;
		case 3:
			return 0x4f;
		case 4:
			return 0x66;
		case 5:
			return 0x6d;
		case 6:
			return 0x7d;
		case 7:
			return 0x07;
		case 8:
			return 0x7f;
		case 9:
			return 0x67;
		default:
			return 0xff;
	}	
}

// Converts an integer to a format suitable for HEX displays by splitting it into individual digits and converting each to BCD.
int writeHEX(int value){
    return (decimal_bcd((value / 1000) % 10) << 24) |
           (decimal_bcd((value / 100) % 10) << 16) | 
           (decimal_bcd((value / 10) % 10) << 8) | 
            decimal_bcd(value % 10);
}

// Writes an integer value to two HEX display pointers, handling values up to 9999 for the first display and the rest on the second.
void writeBoth(int value, volatile int *hex_ptr1, volatile int *hex_ptr2){
    if(value <= 9999){
        *hex_ptr1 = writeHEX(value);
    }
    *hex_ptr2 = writeHEX(value/10000);
    *hex_ptr1 = writeHEX(value%10000);

}