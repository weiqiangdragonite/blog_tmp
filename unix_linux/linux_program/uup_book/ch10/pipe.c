/*
 * pipe.c
 *
 * Demonstrates how to create a pipeline from one process to another
 *
 * Takes two args, each a command, and connects
 * av[1] is output to input of av[2]
 *
 * Usage: pipe command1 command2
 * Effect: command1 | command2
 *
 * Limitations: commands do not take arguments
 * use execlp() since known number of args
 * Note: exchange child and parent and watch fun
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	int pipefd[2];
	int newfd;
	pid_t pid;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <cmd1> <cmd2>\n", argv[0]);
		exit(1);
	}

	if (pipe(pipefd) == -1) {
		perror("pipe() failed");
		exit(1);
	}

	if ((pid = fork()) == -1) {
		perror("foek() failed");
		exit(2);
	}

	/* parent */
	if (pid > 0) {
		close(pipefd[1]);	/* parent close write pipe */

		if (dup2(pipefd[0], 0) == -1) {
			perror("dup2() failed");
			exit(1);
		}
		close(pipefd[0]);	/* stdin is duped */

		execlp(argv[2], argv[2], NULL);
		perror("execlp() failed");
		exit(1);
	}

	/* child */
	close(pipefd[0]);	/* child doesn't read from pipe */

	if (dup2(pipefd[1], 1) == -1) {
		perror("dup2() failed");
		exit(1);
	}
	close(pipefd[1]);	/* stdout is duped */

	execlp(argv[1], argv[1], NULL);
	perror("execlp() failed");
	exit(1);
}

