#
# Makefile template:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = touch

# space-separated list of source files
SRS = init.s clock.s ddr.s
SRC = main.c uart.c

# automatically generated list of object files
OBJS = $(SRS:.s=.o) $(SRC:.c=.o)

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -O2 -c

# compiler to use
CC = arm-linux-gcc
LD = arm-linux-ld
CP = arm-linux-objcopy
DP = arm-linux-objdump
#CC = arm-linux-gcc
#LD = arm-linux-ld
#CP = arm-linux-objcopy
#DP = arm-linux-objdump

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
