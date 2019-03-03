#include <stdio.h>

extern char **environ;
char buf[] = "abcdefg\n";

int
main(int argc, char *argv[])
{
	int i;
	for (i = 0; environ[i] != NULL; ++i)
		printf("environ[%i]: %s\n", i, environ[i]);

	printf("size = %d\n", sizeof(buf));

	return 0;
}
