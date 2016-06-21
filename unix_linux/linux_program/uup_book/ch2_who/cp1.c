/*
 * version 1 of cp - uses read and write with tunable buffer size
 *
 * usage: cp1 src dest
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define BUF_SIZE 4096
#define COPYMODE 0644


int
main(int argc, char *argv[])
{
	int in_fd, out_fd, n_chars;
	char buf[BUF_SIZE];

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <src> <dest>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((in_fd = open(argv[1], O_RDONLY)) == -1) {
		fprintf(stderr, "Cannot open %s: %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	if ((out_fd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, COPYMODE)) == -1) {
		fprintf(stderr, "Cannot create %s: %s\n", argv[2], strerror(errno));
		exit(EXIT_FAILURE);
	}

	while ((n_chars = read(in_fd, buf, BUF_SIZE)) > 0) {
		if (write(out_fd, buf, n_chars) != n_chars) {
			perror("write() failed");
			exit(EXIT_FAILURE);
		}
	}

	if (n_chars == -1) {
		perror("read() failed");
		exit(EXIT_FAILURE);
	}

	if (close(in_fd) == -1 || close(out_fd) == -1)
		perror("close() failed");

	return 0;
}





