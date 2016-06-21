/*
 * psh2.c - shell version 2
 *
 * Solves the 'one-shot' problem of version 1
 * Uses execvp(), but fork() first so that the
 * shell waits around to perform another command
 *
 * BUG: shell creates signals. Run vi, press ^C
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXARGS		20	/* cmdline args */
#define ARGLEN		100	/* token length */


char *make_string(char *buf);

int
main(int argc, char *argv[])
{
	char *arglist[MAXARGS + 1];	/* include NULL */
	int num_args;
	char argbuf[ARGLEN];

	num_args = 0;

	while (num_args < MAXARGS) {
		printf("Arg[%d]? ", num_args);
		if (fgets(argbuf, ARGLEN, stdin) && *argbuf != '\n')
			arglist[num_args++] = make_string(argbuf);
		else {
			if (num_args > 0) {	/* any args? */
				arglist[num_args] = NULL;
				execute(arglist);
				num_args = 0;
			}
		}
	}

	return 0;
}


int
execute(char *arglist[])
{
	pid_t pid;
	int exit_status;

	pid = fork();
	switch (pid) {
	case -1:
		perror("fork() failed");
		exit(1);
	case 0:
		execvp(arglist[0], arglist);
		perror("execvp() failed");
		exit(1);
	default:
		break;
	}

	/* parent go here */
	while (1) {
		if (wait(&exit_status) == pid)
			break;
	}
	printf("Child exited with status %d, %d\n",
		exit_status >> 8, exit_status & 0x7F);
}


char *
make_string(char *buf)
{
	char *cp;

	buf[strlen(buf) - 1] = '\0';
	cp = (char *) malloc(strlen(buf) + 1);
	if (cp == NULL) {
		perror("malloc() failed");
		exit(1);
	}

	strcpy(cp, buf);
	return cp;
}
