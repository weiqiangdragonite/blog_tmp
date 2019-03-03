#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = bsp

# list of source files
SRC_S = start.s
SRC_C = clock.c sdram.c nand.c boot.c uart.c exceptions.c \
        led.c buzzer.c key.c time.c lcd.c adc.c

# automatically generated list of object files
OBJS = $(SRC_S:.s=.o) $(SRC_C:.c=.o)

# compiler to use
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
CP = arm-none-eabi-objcopy
DP = arm-none-eabi-objdump

# include path
INCLUDE = 

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror \
         -nostdlib -fomit-frame-pointer -mcpu=arm1176jzf-s -msoft-float\
         $(INCLUDE) -c

# default target
$(EXE).a : $(OBJS)
	$(AR) -cr -o $@ $^
	cp *.o $(EXE).a ../os_obj

# dependencies
%.o : %.s
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	rm -f core *.o *.a
