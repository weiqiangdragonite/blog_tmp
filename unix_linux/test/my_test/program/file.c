
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int
main(void)
{
	FILE *file = fopen("server.txt", "a");
	if (file == NULL) {
		fprintf(stderr, "Open/Create server log file failed: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	char buf[] = "hello, world";
	fwrite(buf, strlen(buf), sizeof(char), file);
	fclose(file);




	int log_fd = open("server.log",
			O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
	if (log_fd == -1) {
		fprintf(stderr, "Open/Create server log file failed: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	char log_buf[] = "hello, world";
	write(log_fd, log_buf, strlen(log_buf));
	close(log_fd);

	return 0;
}
