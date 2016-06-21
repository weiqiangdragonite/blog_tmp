/*
 * file_tc.c - read the current date/time from a file
 * use fcntl() for locking
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define BUFSIZE		1024
#define oops(m, x)	{ perror(m); exit(x); }

void lock_operation(int fd, int op);


int
main(int argc, char *argv[])
{
	int fd, nread;
	char buf[BUFSIZE];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	if ((fd = open(argv[1], O_RDONLY)) == -1)
		oops(argv[1], 2);

	lock_operation(fd, F_RDLCK);	/* lock for reading */


	while ((nread = read(fd, buf, BUFSIZE)) > 0)
		write(1, buf, nread);

	lock_operation(fd, F_UNLCK);
	close(fd);

	return 0;
}


void
lock_operation(int fd, int op)
{
	struct flock lock;
	lock.l_whence = SEEK_SET;
	lock.l_start = lock.l_len = 0;
	lock.l_pid = getpid();
	lock.l_type = op;

	if (fcntl(fd, F_SETLKW, &lock) == -1)
		oops("lock operation", 6);
}

