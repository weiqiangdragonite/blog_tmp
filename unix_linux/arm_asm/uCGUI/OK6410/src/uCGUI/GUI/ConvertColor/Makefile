#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#
# uCGUI/GUI/ConvertColor

# name for executable
EXE = ConvertColor

# list of source files
SRC_S = 
SRC_C =                 \
        LCDP111.c       \
        LCDP222.c       \
        LCDP233.c       \
        LCDP323.c       \
        LCDP332.c       \
        LCDP444_12.c    \
        LCDP444_12_1.c  \
        LCDP444_16.c    \
        LCDP555.c       \
        LCDP556.c       \
        LCDP565.c       \
        LCDP655.c       \
        LCDP8666.c      \
        LCDP8666_1.c    \
        LCDPM233.c      \
        LCDPM323.c      \
        LCDPM332.c      \
        LCDPM444_12.c   \
        LCDPM444_16.c   \
        LCDPM555.c      \
        LCDPM556.c      \
        LCDPM565.c      \
        LCDPM655.c

# automatically generated list of object files
OBJS = $(SRC_S:.s=.o) $(SRC_C:.c=.o)

# compiler to use
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
CP = arm-none-linux-gnueabi-objcopy
DP = arm-none-linux-gnueabi-objdump

# include path
INCLUDE = -I$(shell pwd)/../Core    -I$(shell pwd)/../WM    \
          -I$(shell pwd)/../JPEG    -I$(shell pwd)/../MultiLayer    \
          -I$(shell pwd)/../Widget  \
          -I$(shell pwd)/../../Config

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer $(INCLUDE) -c

# default target
$(EXE).a : $(OBJS)
	$(AR) -cr -o $@ $^
	cp *.o $(EXE).a ../../uCGUI_obj

# dependencies
%.o : %.s
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	rm -f core *.o *.a