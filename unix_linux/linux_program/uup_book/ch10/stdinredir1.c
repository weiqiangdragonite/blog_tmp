/*
 * stdinredir1.c
 * purpose: show how to redirect standard input by replacing file
 *          descriptor 0 with a connection to a file.
 * action:  reads three lines form standard input, then closed
 *          fd 0, opens a disk file, then reads in three mode
 *          line from stanard input.
 */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int
main(int argc, char *argv[])
{
	int fd;
	char line[100];
	int i;

	/* read std input */
	for (i = 0; i < 3; ++i) {
		fgets(line, 100, stdin);
		printf("%s", line);
	}

	/* redirect input */
	close(0);
	fd = open("/etc/passwd", O_RDONLY);
	if (fd != 0) {
		perror("open() failed");
		exit(1);
	}

	for (i = 0; i < 3; ++i) {
		fgets(line, 100, stdin);
		printf("%s", line);
	}

	return 0;
}




