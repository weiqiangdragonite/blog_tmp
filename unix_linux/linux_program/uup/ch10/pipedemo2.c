/*
 * pipedemo2.c
 *
 * Demonstrates: how pipe is duplicated in fork()
 * Effect:       parent continues to write and read pipe,
 *               but child also writes to pipe
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define CHILD_MESS	"I want a cookie\n"
#define PAR_MESS	"testing...\n"



int
main(int argc, char *argv[])
{
	int pipefd[2];
	char buf[1024];
	int len, read_len;


	/* get a pipe */
	if (pipe(pipefd) == -1) {
		perror("pipe() failed");
		exit(1);
	}

	switch (fork()) {
	case -1:
		perror("fork() failed");
		exit(1);
	case 0:	/* child writes to pipe every 5 seconds */
		len = strlen(CHILD_MESS);
		while (1) {
			if (write(pipefd[1], CHILD_MESS, len) != len) {
				perror("write() failed");
				exit(1);
			}
			sleep(5);
		}
	default:/* parent read from pipe and also writes to pipe */
		len = strlen(PAR_MESS);
		while (1) {
			if (write(pipefd[1], PAR_MESS, len) != len) {
				perror("write() failed");
				exit(1);
			}
			sleep(1);

			read_len = read(pipefd[0], buf, 1024);
			if (read_len <= 0)
				break;
			write(1, buf, read_len);
		}
	}

	return 0;
}

