#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = uCGUI

# space-separated list of source files
SRC_S = 
SRC_C = 

# automatically generated list of object files
OBJS = *.o
uCGUI_DIR = $(shell pwd)/uCGUI_obj

# compiler to use
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
CP = arm-none-linux-gnueabi-objcopy
DP = arm-none-linux-gnueabi-objdump
#GCC_LIB = -L/usr/local/arm/arm-2013.05/lib/gcc/arm-none-linux-gnueabi/4.7.3/

#include path
INCLUDE = 

# lib path
LIB = -L$(shell pwd)/uCGUI_obj

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer $(INCLUDE) -c

# default target
$(EXE).a : $(OBJS)
	cd $(uCGUI_DIR); $(AR) -cr -o $@ $^; cp $(EXE).a ../../os_obj
#	$(AR) -cr -o $@ $^

# dependencies
*.o :
	rm -rf $(uCGUI_DIR); mkdir $(uCGUI_DIR)
	cd GUI_X; make
	cd GUI/AntiAlias; make
	cd GUI/ConvertColor; make
	cd GUI/ConvertMono; make
	cd GUI/Core; make
	cd GUI/Font; make
#	cd GUI/JPEG; make
	cd GUI/LCDDriver; make
	cd GUI/MemDev; make
	cd GUI/MultiLayer; make
	cd GUI/Widget; make
	cd GUI/WM; make
	cd GUIDemo; make

#%.o : %.s
#	$(CC) $(CFLAGS) $^ -o $@

#%.o : %.c
#	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	cd GUI_X; make clean
	cd GUI/AntiAlias; make clean
	cd GUI/ConvertColor; make clean
	cd GUI/ConvertMono; make clean
	cd GUI/Core; make clean
	cd GUI/Font; make clean
	cd GUI/JPEG; make clean
	cd GUI/LCDDriver; make clean
	cd GUI/MemDev; make clean
	cd GUI/MultiLayer; make clean
	cd GUI/Widget; make clean
	cd GUI/WM; make clean
	cd GUIDemo; make clean
	rm -rf $(uCGUI_DIR)