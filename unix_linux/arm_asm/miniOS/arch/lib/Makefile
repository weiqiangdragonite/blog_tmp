#
# Makefile template:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = 

# space-separated list of source files
SRS = switch.s
SRC = exceptions.c clock.c sdram.c uart.c nand.c mmu.c key.c led.c \
      time.c buzzer.c swi.c schedule.c

# automatically generated list of object files
OBJS = $(SRS:.s=.o) $(SRC:.c=.o)

# compiler to use
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
CP = arm-none-linux-gnueabi-objcopy
DP = arm-none-linux-gnueabi-objdump
INCLUDEDIR = $(shell pwd)/../include

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer \
	 -I$(INCLUDEDIR) -c

# default target
#$(EXE).bin : $(OBJS)
#	$(LD) -T $(EXE).lds -o $(EXE).elf $^
#	$(CP) -O binary $(EXE).elf $@
#	$(DP) -D $(EXE).elf > $(EXE).dis

# dependencies
libok6410.a: $(OBJS)
	$(AR) -cr -o $@ $^

%.o : %.s
	$(CC) $(CFLAGS) $^ -o $@
	
%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	rm -f core *.o *.a
