TARGET = Morse
ALT_DEVICE_FAMILY ?= soc_cv_av
SOCEDS_ROOT ?= $(SOCEDS_DEST_ROOT)
HWLIBS_ROOT = $(SOCEDS_ROOT)/ip/altera/hps/altera_hps/hwlib
CROSS_COMPILE = arm-linux-gnueabihf-
CFLAGS = -g -Wall -std=c99 
LDFLAGS =  -g -Wall 
CC = $(CROSS_COMPILE)gcc
ARCH = arm

SRCS = MorseCodeTranslator.c Hardware.c MorseCodeLookupTable.c
OBJS = $(SRCS:.c=.o)

build: $(TARGET)

$(TARGET): Hardware.o MorseCodeTranslator.o MorseCodeLookupTable.o
	$(CC) $(LDFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) *.a *.o *~
