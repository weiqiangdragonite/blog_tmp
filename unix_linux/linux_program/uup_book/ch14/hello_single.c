/*
 * hello_single.c - a single threaded hello world program
 */

#include <stdio.h>

#define NUM	5


int
main(int argc, char *argv[])
{
	void print_msg(char *msg);

	print_msg("hello");
	print_msg("world\n");

	return 0;
}

void
print_msg(char *msg)
{
	int i;

	for (i = 0; i < NUM; ++i) {
		printf("%s", msg);
		fflush(stdout);
		sleep(1);
	}
}

