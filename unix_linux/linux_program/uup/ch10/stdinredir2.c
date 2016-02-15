/*
 * stdinredir2.c
 * shows two more methods for redirecting standard input
 * use # define to set one or the other
 */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define CLOSE_DUP	/* open, close, dup, close */
//#define USE_DUP2	/* open, dup2, close */

int
main(int argc, char *argv[])
{
	int fd, newfd;
	char line[100];
	int i;

	/* read std input */
	for (i = 0; i < 3; ++i) {
		fgets(line, 100, stdin);
		printf("%s", line);
	}

	/* redirect input */
	fd = open("/etc/passwd", O_RDONLY);
	if (fd == -1) {
		perror("open() failed");
		exit(1);
	}
#ifdef CLOSE_DUP
	close(0);
	newfd = dup(fd);	/* copy open fd to 0 */
#else
	newfd = dup2(fd, 0);	/* close 0, dup fd to 0 */
#endif

	if (newfd != 0) {
		fprintf(stderr, "Could not duplicate fd to 0\n");
		exit(1);
	}

	close(fd);		/* close original fd */


	/* read and input */
	for (i = 0; i < 3; ++i) {
		fgets(line, 100, stdin);
		printf("%s", line);
	}

	return 0;
}




