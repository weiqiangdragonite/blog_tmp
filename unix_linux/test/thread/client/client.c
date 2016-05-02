/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "inet_socket.h"


#define SERVER		"10.192.152.67"
#define PORT_NUM	"8000"

int
main(int argc, char *argv[])
{
	int socket_fd;
	char buffer[1024];
	ssize_t num_read;

	socket_fd = inet_connect(SERVER, PORT_NUM, SOCK_STREAM);

	if (socket_fd == -1) {
		fprintf(stderr, "inet_connect() failed\n");
		exit(EXIT_FAILURE);
	}
	printf("connect success\n");

	while (1) {
		num_read = read(socket_fd, buffer, 1024);
		if (num_read == -1) {
			fprintf(stderr, "read() failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		printf("num_read = %d\n", (int) num_read);
		if (num_read != 0)
			printf("recv from server: %s\n", buffer);
	}

	return 0;
}
