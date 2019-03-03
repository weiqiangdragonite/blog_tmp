#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = test

# space-separated list of source files
SRC_S = start.s
SRC_C = main.c

# automatically generated list of object files
OBJS = $(SRC_S:.s=.o) $(SRC_C:.c=.o)

# compiler to use
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
CP = arm-none-linux-gnueabi-objcopy
DP = arm-none-linux-gnueabi-objdump

#include path
INCLUDE = -I/usr/local/arm/CodeSourcery/arm-none-linux-gnueabi-2013.05

# lib path
LIB = -L/usr/local/arm/CodeSourcery/arm-none-linux-gnueabi-2013.05

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -mcpu=arm1176jzf-s -static \
         -fomit-frame-pointer $(INCLUDE) -c

# default target
$(EXE).bin : $(OBJS)
	$(LD) -v -T $(EXE).lds -o $(EXE).elf $^
	$(CP) -O binary $(EXE).elf $@
	$(DP) -D $(EXE).elf > $(EXE).dis

# dependencies
%.o : %.s
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIB)

# housekeeping
.PHONY : clean
clean:
	rm -f core *.o *.bin *.elf *.dis
