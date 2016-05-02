#
# Makefile:
#
# By: weiqiangdragonite@gmail.com
#

# name for executable
EXE = server

# space-separated list of source files
SRCS = server.c inet_socket.c

# space-separated list of header files
HDRS = 

# automatically generated list of object files
OBJS = $(SRCS:.c=.o)

# compiler to use

# include path: -I
INCLUDE = 

# lib path: -L
LIB_PATH = 

# lib: -l
LIB = -lpthread

# flags to pass compiler
CFLAGS = 

# default target
$(EXE) : $(OBJS)
	gcc $^ $(LIB_PATH) $(LIB) -o $@

# dependencies
%.o : %.c
	gcc -c $(CFLAGS) $(INCLUDE) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	rm -rf *.o core $(EXE)

