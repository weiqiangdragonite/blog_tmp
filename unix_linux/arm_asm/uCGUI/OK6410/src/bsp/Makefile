#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = 

# list of source files
SRC_S = start.s
SRC_C = clock.c sdram.c nand.c uart.c boot.c time.c led.c exceptions.c \
        lcd.c key.c

# automatically generated list of object files
OBJS = $(SRC_S:.s=.o) $(SRC_C:.c=.o)

# compiler to use
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
CP = arm-none-linux-gnueabi-objcopy
DP = arm-none-linux-gnueabi-objdump

# include path
INCLUDE = -I$(shell pwd)/../uCGUI/GUI/Core    -I$(shell pwd)/../uCGUI/GUI/WM    \
          -I$(shell pwd)/../uCGUI/GUI/JPEG    -I$(shell pwd)/../uCGUI/GUI/MultiLayer    \
          -I$(shell pwd)/../uCGUI/GUI/Widget  \
          -I$(shell pwd)/../uCGUI/Config

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer $(INCLUDE) -c

# default target
bsp.a : $(OBJS)
	$(AR) -cr -o $@ $^
	cp *.o bsp.a ../os_obj

# dependencies
%.o : %.s
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	rm -f core *.o *.a