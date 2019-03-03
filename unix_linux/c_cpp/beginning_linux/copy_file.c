#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int
main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s file_in file_out\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	char c;

	int in = open(argv[1], O_RDONLY);
	int out = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC,
			S_IRUSR | S_IWUSR);
	while (read(in, &c, 1) == 1)
		write(out, &c, 1);

	return 0;
}
