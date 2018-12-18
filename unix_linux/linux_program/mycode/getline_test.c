/*
 * getline() demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t res = 0;

	FILE *fp = fopen("/tmp/test.txt", "r");
	if (fp == NULL) {
		fprintf(stderr, "cannot open /tmp/test.txt\n", strerror(errno));
		return -1;
	}

	while (1) {
		res = getline(&line, &len, fp);
		if (res == -1) { // end of file 也是返回-1
			fprintf(stderr, "getline() failed\n");
			break;
		} else {
			line[strlen(line)-1] = '\0';
			printf("len = %d\n", len); // 
			printf("line = %s\n", line);
		}
	}

	if (line != NULL)
		free(line);
	fclose(fp);

	return 0;
}

