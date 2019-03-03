#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#
# uCGUI/GUI/Font

# name for executable
EXE = Font

# list of source files
SRC_S = 
SRC_C =                 \
        F08_1.c         \
        F08_ASCII.c     \
        F10S_1.c        \
        F10S_ASCII.c    \
        F10_1.c         \
        F10_ASCII.c     \
        F13B_1.c        \
        F13B_ASCII.c    \
        F13HB_1.c       \
        F13HB_ASCII.c   \
        F13H_1.c        \
        F13H_ASCII.c    \
        F13_1.c         \
        F13_ASCII.c     \
        F16B_1.c        \
        F16B_ASCII.c    \
        F16_1.c         \
        F16_1HK.c       \
        F16_ASCII.c     \
        F16_HK.c        \
        F24_1.c         \
        F24_ASCII.c     \
        F24B_1.c        \
        F24B_ASCII.c    \
        F32_1.c         \
        F32_ASCII.c     \
        F32B_1.c        \
        F32B_ASCII.c    \
        F4x6.c          \
        F6x8.c          \
        F8x10_ASCII.c   \
        F8x12_ASCII.c   \
        F8x13_1.c       \
        F8x13_ASCII.c   \
        F8x15B_1.c      \
        F8x15B_ASCII.c  \
        F8x16.c         \
        F8x8.c          \
        FComic18B_1.c   \
        FComic18B_ASCII.c   \
        FComic24B_1.c       \
        FComic24B_ASCII.c   \
        FD24x32.c       \
        FD32.c          \
        FD36x48.c       \
        FD48.c          \
        FD48x64.c       \
        FD60x80.c       \
        FD64.c          \
        FD80.c

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