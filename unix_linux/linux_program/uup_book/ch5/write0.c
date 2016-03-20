/*
 * write0.c
 *
 * purpose: send messages to another terminal
 * method: open the other terminal for output then
 *         copy from stdin to that terminal
 * shows: a terminal is just a file supporting regular i/o
 * usage: write0 ttyname
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>


#define BUF_SIZE	1024

int
main(int argc, char *argv[])
{
	int fd;
	char buf[BUF_SIZE];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <ttyname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_WRONLY);
	if (fd == -1) {
		perror("open() failed");
		exit(EXIT_FAILURE);
	}

	while (fgets(buf, BUF_SIZE, stdin) != NULL) {
		if (write(fd, buf, strlen(buf)) == -1) {
			perror("write() failed");
			exit(EXIT_FAILURE);
		}
	}
	close(fd);
	return 0;
}







