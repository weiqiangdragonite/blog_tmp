#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = ok6410-uCOS-II

# space-separated list of source files
SRC_S = 
SRC_C = 

# automatically generated list of object files
OBJS = *.o
OBJS_DIR = $(shell pwd)/os_obj

# compiler to use
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
CP = arm-none-eabi-objcopy
DP = arm-none-eabi-objdump
GCC_LIB = -L/usr/local/CodeSourcery/arm-2013.11/lib/gcc/arm-none-eabi/4.8.1/

#include path
INCLUDE = 

# lib path
LIB = -L$(shell pwd)/os_obj

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer $(INCLUDE) -c

# default target
$(EXE).bin : $(OBJS)
	cp $(EXE).lds $(OBJS_DIR); cd $(OBJS_DIR); \
	$(LD) -T $(EXE).lds -o $(EXE).elf *.o $(GCC_LIB) -lgcc; \
	cp $(EXE).elf ../
	$(CP) -O binary $(EXE).elf $@
	$(DP) -D $(EXE).elf > $(EXE).dis
	rm -rf $(OBJS_DIR)

# dependencies
*.o :
	rm -rf $(OBJS_DIR); mkdir $(OBJS_DIR)
	cd bsp; make
	cd arm; make
	cd kernel; make
	cd app; make

#%.o : %.s
#	$(CC) $(CFLAGS) $^ -o $@

#%.o : %.c
#	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	cd bsp; make clean
	cd arm; make clean
	cd kernel; make clean
	cd app; make clean
	rm -f core *.bin *.elf *.dis; rm -rf $(OBJS_DIR)
