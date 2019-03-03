#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#
# uCGUI/GUIDemo

# name for executable
EXE = GUIDemo

# list of source files
SRC_S = 
SRC_C =                 \
        GUIDEMO.c   \
        GUIDEMO_Automotive.c   \
        GUIDEMO_Bitmap.c   \
        GUIDEMO_Bitmap4bpp.c   \
        GUIDEMO_Circle.c   \
        GUIDEMO_ColorBar.c   \
        GUIDEMO_ColorList.c   \
        GUIDEMO_Cursor.c   \
        GUIDEMO_Dialog.c   \
        GUIDEMO_Font.c   \
        GUIDEMO_FrameWin.c   \
        GUIDEMO_Graph.c   \
        GUIDEMO_HardwareInfo.c   \
        GUIDEMO_Intro.c   \
        GUIDEMO_LUT.c   \
        GUIDEMO_MemDevB.c   \
        GUIDEMO_Navi.c   \
        GUIDEMO_Polygon.c   \
        GUIDEMO_ProgBar.c   \
        GUIDEMO_Speed.c   \
        GUIDEMO_Touch.c   \
        SeggerLogoBlue.c

# automatically generated list of object files
OBJS = $(SRC_S:.s=.o) $(SRC_C:.c=.o)

# compiler to use
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
CP = arm-none-linux-gnueabi-objcopy
DP = arm-none-linux-gnueabi-objdump

# include path
INCLUDE = -I$(shell pwd)/../GUI/Core    -I$(shell pwd)/../GUI/WM    \
          -I$(shell pwd)/../GUI/JPEG    -I$(shell pwd)/../GUI/MultiLayer    \
          -I$(shell pwd)/../GUI/Widget  \
          -I$(shell pwd)/../Config

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer $(INCLUDE) -c

# default target
$(EXE).a : $(OBJS)
	$(AR) -cr -o $@ $^
	cp *.o $(EXE).a ../uCGUI_obj

# dependencies
%.o : %.s
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	rm -f core *.o *.a