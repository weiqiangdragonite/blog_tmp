/*
 * psh1.c - shell version 1
 * Prompts for the command and its arguments.
 * Builds the argument vector for the call to execvp.
 * Use execvp() , and never returns.
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
	execvp(arglist[0], arglist);
	/* error if go here */
	perror("execvp() failed");
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
