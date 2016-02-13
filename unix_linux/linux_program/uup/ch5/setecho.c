/*
 * setecho.c
 * Usage: setecho <y/n>
 * shows: how to read, change, reset tty attributes
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

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <y/n>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* get attributes */
	if ((val = tcgetattr(0, &info)) == -1) {
		perror("tcgetattr() failed");
		exit(EXIT_FAILURE);
	}

	if (argv[1][0] == 'y')
		info.c_lflag |= ECHO;	/* turn on bit */
	else
		info.c_lflag &= ~ECHO;	/* turn off bit */

	/* set attributes */
	if (tcsetattr(0, TCSANOW, &info) == -1) {
		perror("tcsetattr() failed");
		exit(EXIT_FAILURE);
	}

	return 0;
}



