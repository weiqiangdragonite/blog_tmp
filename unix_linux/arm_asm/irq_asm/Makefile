#
# Makefile template:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = irq

# space-separated list of source files
SRS = init.s clock.s ddr.s uart.s
SRC =

# automatically generated list of object files
OBJS = $(SRS:.s=.o) $(SRC:.c=.o)

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -O2 -c

# compiler to use
CC = arm-linux-gnueabi-gcc
LD = arm-linux-gnueabi-ld
CP = arm-linux-gnueabi-objcopy
DP = arm-linux-gnueabi-objdump

# default target
$(EXE).bin : $(OBJS)
	$(LD) -T $(EXE).lds -o $(EXE).elf $^
	$(CP) -O binary $(EXE).elf $@
	$(DP) -D $(EXE).elf > $(EXE).dis

# dependencies
%.o : %.s
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	rm -f core *.bin *.o *.elf *.dis
