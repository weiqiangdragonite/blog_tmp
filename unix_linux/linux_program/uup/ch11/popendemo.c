/*
 * popendemo.c
 * demonstrates how to open a program for staindard io
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int
main(int argc, char *argv[])
{
	FILE *fp;
	char buf[100];
	int i = 0;

	fp = popen("who | sort", "r");	/* open the command */
	/* read form comand */
	while (fgets(buf, 100, fp) != NULL)
		printf("%3d: %s", i++, buf);

	pclose(fp);
	return 0;
}



