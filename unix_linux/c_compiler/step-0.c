/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>


int token = 0;		/* current token */
char *src = NULL;	/* pointer to source code string */
char *old_src = NULL;	/* pointer to source code string */
int poolsize = 0;	/* default size of text/data/stack */
int line = 0;		/* line number */


int
main(int argc, char *argv[])
{
	int i, fd;

	--argc;
	++argv;

	/* arbitrary size */
	poolsize = 256 * 1024;
	line = 1;

	if ((fd = open(*argv, O_RDONLY)) < 0) {
		fprintf(stderr, "could not open %s\n", *argv);
		return -1;
	}

	if ((src = malloc(poolsize)) == NULL) {
		fprintf(stderr, "could not malloc %d for source area\n", poolsize);
		return -1;
	}
	old_src = src;

	/* read the source file */
	if ((i = read(fd, src, poolsize - 1)) <= 0) {
		fprintf(stderr, "read return %d\n", i);
		return -1;
	}
	/* add EOF */
	src[i] = '\0';
	close(fd);

	program();
	return eval();
}


void
next(void)
{
	token = *src++;
	return;
}

void
expression(int level)
{
	/* do nothing */
}

void
program(void)
{
	next();	/* get next token */
	while (token > 0) {
		printf("token is: %c\n", token);
		next();
	}
}

int
eval(void)
{
	/* do nothing */
	return 0;
}
