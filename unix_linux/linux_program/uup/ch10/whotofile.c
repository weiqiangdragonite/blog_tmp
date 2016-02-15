/*
 * whotofile.c
 * show how to redirect output for another program
 * fork, then in the child, redirect output, then exec
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	pid_t pid;
	int fd;

	printf("About to run who into a file\n");

	if ((pid = fork()) == -1) {
		perror("fork() failed");
		exit(1);
	}

	if (pid == 0) {
		close(1);
		fd = creat("list", 0644);
		execlp("who", "who", NULL);
		perror("execlp() failed");
		exit(1);
	}

	wait(NULL);
	printf("Done\n");

	return 0;
}

