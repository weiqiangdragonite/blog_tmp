


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
#define PORT		"10000"

int
main(int argc, char *argv[])
{
	int socket_fd;
	char buffer[1024];
	ssize_t num_read;

	int server[FD_SETSIZE];
	fd_set readfds, allfds;
	struct timeval timeout;


	timeout.tv_sec = 1; /* seconds */
	timeout.tv_usec = 0; /* microseconds */


	printf("Connecting to server\n");

	while (1) {
		socket_fd = inet_connect(SERVER, PORT, SOCK_STREAM);
		if (socket_fd == -1) {
			fprintf(stderr, "inet_connect() failed\n");
			sleep(1);
			continue;
		}

		// 初始化select
		maxfd = socket_fd;
		maxi = -1;		/* index into client[] array */
		// 
		for (i = 0; i < FD_SETSIZE; i++)
			server[i] = -1;
		FD_ZERO(&allfds);		//清空
		FD_SET(socket_fd, &allfds);	//将监听socket加入select检测的描述符集合

		while (1) {
			readfds = allfds;
			nready = select(maxfd + 1, &readfds, NULL, NULL, &timeout);

			// timeout
			if (nready == 0) {
				fprintf(stderr, "select() timeout\n");
				sleep(1);
				continue;
			} else if (nready == -1) {
				fprintf(stderr, "select() failed: %s\n", strerror(errno));
				sleep(1);
				continue;
			}

			// connect to  new server
			if (FD_ISSET(socket_fd, &readfds)) {
				for (i = 0; i < FD_SETSIZE; ++i) {
					/* save connect server fd */
					if (server[i] < 0) {
						printf("Connected to server, i = %d\n", i);
						server[i] = socket_fd;
						break;
					}
				}
			}






			memset(buffer, 0, sizeof(buffer));

			write(socket_fd, data, sizeof(data));

			num_read = read(socket_fd, buffer, sizeof(buffer));
			if (num_read == -1) {
				fprintf(stderr, "read() failed: %s\n",
					strerror(errno));
				exit(EXIT_FAILURE);
			}

			if (num_read != 0)
				printf("read from server(%d bytes):\n%s\n\n",
					(int) num_read, buffer);
			else
				break;

			sleep(1);
		}

		close(socket_fd);
		socket_fd = -1;
	}

	return 0;
}
