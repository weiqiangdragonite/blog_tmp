#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#
# uCGUI/GUI/AntiAlias

# name for executable
EXE = AntiAlias

# list of source files
SRC_S = 
SRC_C =                 \
        GUIAAArc.c      \
        GUIAAChar.c     \
        GUIAAChar2.c    \
        GUIAAChar4.c    \
        GUIAACircle.c   \
        GUIAALib.c      \
        GUIAALine.c     \
        GUIAAPoly.c     \
        GUIAAPolyOut.c

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