#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#
# uCGUI/GUI/MemDev

# name for executable
EXE = MemDev

# list of source files
SRC_S = 
SRC_C =                 \
        GUIDEV.c        \
        GUIDEV_1.c      \
        GUIDEV_8.c      \
        GUIDEV_16.c     \
        GUIDEV_AA.c     \
        GUIDEV_Auto.c   \
        GUIDEV_Banding.c    \
        GUIDEV_Clear.c      \
        GUIDEV_CmpWithLCD.c \
        GUIDEV_CopyFromLCD.c    \
        GUIDEV_CreateFixed.c    \
        GUIDEV_GetDataPtr.c \
        GUIDEV_GetXSize.c   \
        GUIDEV_GetYSize.c   \
        GUIDEV_Measure.c    \
        GUIDEV_ReduceYSize.c    \
        GUIDEV_SetOrg.c \
        GUIDEV_Usage.c  \
        GUIDEV_UsageBM.c    \
        GUIDEV_Write.c  \
        GUIDEV_WriteAlpha.c \
        GUIDEV_WriteEx.c    \
        GUIDEV_XY2PTR.c

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