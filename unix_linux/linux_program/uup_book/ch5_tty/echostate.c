/*
 * echostate.c
 * reports current state of echo bit in tty driver for fd 0
 * shows how to read attributes from dirver and test a bit
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

int
main(int argc, char *argv[])
{
	struct termios info;
	int val;

	val = tcgetattr(0, &info);
	if (val == -1) {
		perror("tcgetattr() failed");
		exit(EXIT_FAILURE);
	}

	if (info.c_lflag & ECHO)
		printf("echo is on\n");
	else
		printf("echo is off\n");

	return 0;
}



