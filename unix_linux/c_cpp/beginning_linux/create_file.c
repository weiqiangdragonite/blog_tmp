#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int
main(int argc, char *argv[])
{
	/*  */
	if (argc != 3) {
		fprintf(stderr, "Usage: %s name size\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* File name */
	char *name = argv[1];
	/* File size */
	unsigned int size = atoi(argv[2]) * 1024 * 1024;

	//char *buf = malloc(sizeof(char) * size);

	/* Create file */
	int fd = open(name, O_CREAT | O_WRONLY | O_TRUNC,
			S_IRWXU | S_IRGRP | S_IROTH);
	if (fd == -1) {
		fprintf(stderr, "Cannot create %s: %s\n",
			name, strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Write data */
	int i;
	char c = 'A';
	for (i = 0; i < size; ++i) {
		if (write(fd, &c, 1) != 1) {
			close(fd);
//			free(buf);
			fprintf(stderr, "Cannot write data: %s\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	close(fd);
//	free(buf);

	return 0;
}
