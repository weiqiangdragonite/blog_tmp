#
# Makefile template:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = miniOS

# space-separated list of source files
SRS = start.s
SRC = main.c

# automatically generated list of object files
OBJS = $(SRS:.s=.o) $(SRC:.c=.o) arch/lib/libok6410.a

# compiler to use
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
CP = arm-none-linux-gnueabi-objcopy
DP = arm-none-linux-gnueabi-objdump
INCLUDEARCH = $(shell pwd)/arch/include

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer \
	-I$(INCLUDEARCH) -c

# default target
$(EXE).bin : $(OBJS)
	$(LD) -T $(EXE).lds -o $(EXE).elf $^ -L$(INCLUDEARCH)/../lib 
	$(CP) -O binary $(EXE).elf $@
	$(DP) -D $(EXE).elf > $(EXE).dis

# dependencies
arch/lib/libok6410.a:
	cd arch/lib; make

%.o : %.s
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	cd arch/lib; make clean
	rm -f core *.bin *.o *.elf *.dis
