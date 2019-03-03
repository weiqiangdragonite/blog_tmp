#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = 

# list of source files
SRC_S = 
SRC_C = app.c atk-hc05.c

# automatically generated list of object files
OBJS = $(SRC_S:.s=.o) $(SRC_C:.c=.o)

# compiler to use
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
CP = arm-none-eabi-objcopy
DP = arm-none-eabi-objdump

# include path
INCLUDE = 

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer $(INCLUDE) -c

# default target
app.a : $(OBJS)
	$(AR) -cr -o $@ $^
	cp *.o app.a ../os_obj

# dependencies
%.o : %.s
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	rm -f core *.o *.a
