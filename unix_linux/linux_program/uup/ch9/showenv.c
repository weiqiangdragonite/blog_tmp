/*
 * showenv.v - shows how to read and print the environment
 */

#include <stdio.h>


extern char **environ;	/* points to the array of strings */


int
main(int argc, char *argv[])
{
	int i;

	for (i = 0; environ[i]; ++i)
		printf("%s\n", environ[i]);

	return 0;
}



