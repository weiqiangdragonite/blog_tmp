/*
 * pipedemo.c
 *
 * Demonstrates: how to create and use a pipe
 * Effect:       creates a pipe, writes into writing
 *               end, then runs around and reads from reading
 *               end. A little weird.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int
main(int argc, char *argv[])
{
	int len, i, apipe[2];
	char buf[1024];

	/* get a pipe */
	if (pipe(apipe) == -1) {
		perror("pipe() failed");
		exit(1);
	}

	printf("Got a pipe, {%d, %d}\n", apipe[0], apipe[1]);

	/* read from stdin, write into pipe, read form pipe, print */
	while (fgets(buf, 1024, stdin)) {
		len = strlen(buf);

		if (write(apipe[1], buf, len) != len) {
			perror("write() failed");
			exit(1);
		}

		for (i = 0; i < len; ++i)
			buf[i] = 'X';

		len = read(apipe[0], buf, 1024);
		if (len == -1) {
			perror("read() failed");
			exit(1);
		}

		if (write(1, buf, len) != len) {
			perror("write() failed");
			exit(1);
		}
	}

	return 0;
}

