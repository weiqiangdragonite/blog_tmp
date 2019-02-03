#

CC = gcc
#Flags=-o2 -g -Wall
Flags =
CFLAGS =
INCLUDE = -lcrypto

all: test

#
test: raw_main.o
	$(CC) $(Flags) $(INCLUDE) -o $@  $^

%.o: %.c
	@$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

clean:
	rm -f *.o test
