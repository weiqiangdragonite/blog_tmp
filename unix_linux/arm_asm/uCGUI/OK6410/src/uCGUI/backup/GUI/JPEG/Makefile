#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#
# uCGUI/GUI/JPEG

# name for executable
EXE = JPEG

# list of source files
SRC_S = 
SRC_C =                 \
        GUI_JPEG.c      \
        jcomapi.c       \
        jdapimin.c      \
        jdapistd.c      \
        jdcoefct.c      \
        jdcolor.c       \
        jddctmgr.c      \
        jdhuff.c        \
        jdinput.c       \
        jdmainct.c      \
        jdmarker.c      \
        jdmaster.c      \
        jdmerge.c       \
        jdphuff.c       \
        jdpostct.c      \
        jdsample.c      \
        jdtrans.c       \
        jerror.c        \
        jfdctflt.c      \
        jfdctfst.c      \
        jfdctint.c      \
        jidctflt.c      \
        jidctfst.c      \
        jidctint.c      \
        jidctred.c      \
        jmemmgr.c       \
        jquant1.c       \
        jquant2.c       \
        jutils.c

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