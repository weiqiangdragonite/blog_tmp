#
# Makefile template:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = led

# space-separated list of source files
SRS = crt0.s
SRC = lib.c main.c

# automatically generated list of object files
OBJS = $(SRS:.s=.o) $(SRC:.c=.o)

# compiler to use
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
CP = arm-none-linux-gnueabi-objcopy
DP = arm-none-linux-gnueabi-objdump

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib  -fomit-frame-pointer -c

# default target
$(EXE).bin : $(OBJS)
	$(LD) -Ttext 0 -o $(EXE).elf $^
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
