/*
 * exec1.c - shows how easy it is for a program to run a program
 */

#include <stdio.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	char *arglist[3];

	arglist[0] = "ls";
	arglist[1] = "-l";
	arglist[2] = NULL;

	printf("Start to execute ls -l\n");
	execvp("ls", arglist);
	printf("ls is done\n");

	return 0;
}

