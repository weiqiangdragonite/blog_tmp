#
# Makefile template:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = 

# list of source files
SRC_S = 
SRC_C = main.c exceptions.c clock.c sdram.c uart.c nand.c mmu.c key.c led.c \
        time.c buzzer.c swi.c schedule.c

# automatically generated list of object files
OBJS = $(SRC_S:.s=.o) $(SRC_C:.c=.o)

# compiler to use
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
CP = arm-none-linux-gnueabi-objcopy
DP = arm-none-linux-gnueabi-objdump

# include path
INCLUDE = -I$(shell pwd)/../include

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer $(INCLUDE) -c

# default target
os_c.a : $(OBJS)
	$(AR) -cr -o $@ $^
	cp *.o os_c.a ../os_obj

# dependencies
%.o : %.s
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	rm -f core *.o *.a
