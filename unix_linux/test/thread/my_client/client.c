/*
 * 当客户端连接上服务器端后，要不停地读取服务器端的数据，如果与服务器端的连接断掉了，
 * 客户端应继续请求与服务器端的连接。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "inet_socket.h"


#define SERVER		"localhost"
#define SEND_PORT	"18888"
#define RECEIVE_PORT	"19999"

int
main(int argc, char *argv[])
{
	int socket_fd;
	char buffer[1024];
	ssize_t num_read;

	signal(SIGPIPE, SIG_IGN);

	while (1) {
		printf("Connecting to server\n");
		socket_fd = inet_connect(SERVER, SEND_PORT, SOCK_STREAM);
		if (socket_fd == -1) {
			fprintf(stderr, "inet_connect() failed\n");
			sleep(1);
			continue;
		}
		printf("Connected to server\n");

		while (1) {
			memset(buffer, 0, sizeof(buffer));

			num_read = read(socket_fd, buffer, sizeof(buffer));
			if (num_read == -1) {
				fprintf(stderr, "read() failed: %s\n",
					strerror(errno));
				exit(EXIT_FAILURE);
			}

			printf("num_read = %d\n", (int) num_read);
			if (num_read != 0)
				printf("recv from server:\n%s\n", buffer);
			else
				break;
		}

		close(socket_fd);
		socket_fd = -1;
	}

	return 0;
}
