#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = 

# list of source files
SRC_S = 
#SRC_C = os_core.c os_flag.c os_mbox.c os_mem.c os_mutex.c os_q.c os_sem.c \
#        os_task.c os_time.c \
#        os_tmr.c os_dbg.c
SRC_C = ucos_ii.c os_dbg.c

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
kernel.a : $(OBJS)
	$(AR) -cr -o $@ $^
	cp *.o kernel.a ../os_obj

# dependencies
%.o : %.s
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	rm -f core *.o *.a
