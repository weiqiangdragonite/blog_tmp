/*

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "inet_socket.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif


#define SEND_PORT	"8000"

int
main(int argc, char *argv[])
{
	struct sockaddr_storage client_addr;
	socklen_t server_addrlen, client_addrlen;
	int socket_fd;
	int client_fd;

	socket_fd = inet_listen(SEND_PORT, SOMAXCONN, &server_addrlen);
	if (socket_fd == -1) {
		fprintf(stderr, "inet_listen() failed.\n");
		exit(EXIT_FAILURE);
	}

	/* Start waiting for client to connect */
	while (1) {
		client_addrlen = sizeof(struct sockaddr_storage);
		client_fd = accept(socket_fd, (struct sockaddr *) &client_addr,
				&client_addrlen);

		if (client_fd == -1) {
			fprintf(stderr, "accept() client failed: %s\n",
				strerror(errno));
			continue;
		}

		/* Get client connect ip address */
		char buf[BUF_SIZE];
		snprintf(buf, BUF_SIZE, "Get send connection from client, "
			"client address: %s",
			inet_addrstr((struct sockaddr *) &client_addr,
				client_addrlen));
		printf("%s\n", buf);

		char *str = "hello, world\n";
		ssize_t num_bytes;
		int tmp_errno;

		while (1) {
			tmp_errno = errno;
			errno = 0;
			num_bytes = write(client_fd, str, strlen(str));
			if (num_bytes != strlen(str)) {
				if (errno == EPIPE) {
					close(client_fd);
					client_fd = -1;
					errno = tmp_errno;
					printf("Client close connection.\n");
					break;
				} else
					fprintf(stderr, "write() failed: %s\n",
						strerror(errno));
			}

			sleep(5);
		}

		continue;
	}
	close(socket_fd);


	return 0;
}

