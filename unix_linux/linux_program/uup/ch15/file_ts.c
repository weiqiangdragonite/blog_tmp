/*
 * file_ts.c - read the current date/time from a file
 * use fcntl() for locking
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define oops(m, x)	{ perror(m); exit(x); }

void lock_operation(int fd, int op);


int
main(int argc, char *argv[])
{
	int fd;
	time_t now;
	char *msg;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	if ((fd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1)
		oops(argv[1], 2);

	while (1) {
		time(&now);
		msg = ctime(&now);

		lock_operation(fd, F_WRLCK);	/* lock for writing */

		if (lseek(fd, 0, SEEK_SET) == -1)
			oops("lseek", 3);
		if (write(fd, msg, strlen(msg)) == -1)
			oops("write", 4);

		lock_operation(fd, F_UNLCK);	/* unlock file */
		sleep(1);
	}

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

