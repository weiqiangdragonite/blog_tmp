
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>

int
main(int argc, char *argv[])
{
	fd_set global_reads, reads;
	int result, str_len;
	char buf[1024];
	struct timeval timeout;

	FD_ZERO(&global_reads);
	FD_SET(0, &global_reads);	// 0 is standard input

	while (1) {
		reads = global_reads;
	
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;

		result = select(1, &reads, NULL, NULL, &timeout);
		if (result == -1) {
			fprintf(stderr, "select() failed\n");
			break;
		} else if (result == 0) {
			printf("time out!\n");
			continue;
		}

		if (FD_ISSET(0, &reads)) {
			str_len = read(0, buf, sizeof(buf));
			buf[str_len] = '\0';
			printf("msg from console: %s", buf);
		}
	}

	return 0;
}



