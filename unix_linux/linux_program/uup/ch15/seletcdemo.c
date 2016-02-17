/*
 * selectdemo.c: watch for input on two devices AND timeout
 * usage: selectdemo dev1 dev2 timeout
 * action: reports on input from each file, and reports timeouts
 */

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


#define oops(m, x)	{ perror(m); exit(x); }
#define BUFSIZE		1024

void showdata(char *fname, int fd);

int
main(int argc, char *argv[])
{
	int fd1, fd2;
	struct timeval timeout;
	fd_set readfds;
	int maxfd;
	int retval;

	if (argc != 4) {
		fprintf(stderr, "Usage: %s file file timeout\n", argv[0]);
		exit(1);
	}

	/* open files */
	if ((fd1 = open(argv[1], O_RDONLY)) == -1)
		oops(argv[1], 2);
	if ((fd2 = open(argv[2], O_RDONLY)) == -1)
		oops(argv[1], 3);
	maxfd = 1 + (fd1 > fd2 ? fd1 : fd2);

	while (1) {
		/* make a list of file descriptors to watch */
		FD_ZERO(&readfds);
		FD_SET(fd1, &readfds);
		FD_SET(fd2, &readfds);

		/* set timeout */
		timeout.tv_sec = atoi(argv[3]);
		timeout.tv_usec = 0;

		/* wait for input */
		retval = select(maxfd, &readfds, NULL, NULL, &timeout);
		if (retval == -1)
			oops("select() failed", 4);

		if (retval > 0) {
			/* check bits for each fd */
			if (FD_ISSET(fd1, &readfds))
				showdata(argv[1], fd1);
			if (FD_ISSET(fd2, &readfds))
				showdata(argv[2], fd2);
		} else
			printf("No input after %d seconds\n", atoi(argv[3]));
	}
	close(fd1);
	close(fd2);

	return 0;
}


void
showdata(char *fname, int fd)
{
	char buf[BUFSIZE];
	int n;

	printf("%s: ", fname);
	fflush(stdout);

	n = read(fd, buf, BUFSIZE);
	if (n == -1)
		oops("read() failed", 5);
	write(1, buf, n);
	write(1, "\n", 1);
}



