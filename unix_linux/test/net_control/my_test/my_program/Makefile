#
# Makefile:
#
# By: weiqiangdragonite@gmail.com
#

# name for executable
EXE = ne_server

# space-separated list of source files
SRC = main.c \
	http_pcap.c \
	tcp_pcap.c \
	circular_buffer.c \
	mysql_func.c \
	public_func.c

# space-separated list of header files
HDRS = circular_buffer.h \
	packet_info.h \
	config.h

# automatically generated list of object files
OBJS = $(SRC:.c=.o)

# obj directory
OBJS_DIR = $(shell pwd)/os_obj

# compiler to use
CC = gcc


# include path: -I
INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include/ -I/usr/include/mysql

# lib path: -L
LIB_PATH = -L/usr/lib64/mysql

# lib: -l
LIB = -lpcap -lpthread -lglib-2.0 -lgthread-2.0 -lmysqlclient -lpcre

# flags to pass compiler
# used for release: -fomit-frame-pointer -O2
CFLAGS = -Wall

# default target
$(EXE) : $(OBJS)
	$(CC) *.o $(LIB_PATH) $(LIB) -o $(EXE)
	echo 'Compile done!'

# dependencies
%.o : %.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	rm -f $(OBJS)
	rm -f $(EXE)

