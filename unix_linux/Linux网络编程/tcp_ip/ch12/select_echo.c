#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>


#define BUF_SIZE	1024
#define BACK_LOG	128



/*
 * create IPv4 TCP socket for server
 */
int
create_socket(const char *port)
{
	struct sockaddr_in svaddr;
	int fd, opt;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		fprintf(stderr, "socket() failed: %s\n", strerror(errno));
		return -1;
	}

	opt = 1;
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
		fprintf(stderr, "setsockopt() failed: %s\n", strerror(errno));  
		return -1;
	}

	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svaddr.sin_port = htons(atoi(port));


	if (bind(fd, (struct sockaddr *) &svaddr, sizeof(svaddr)) == -1) {
		fprintf(stderr, "bind() failed: %s\n", strerror(errno));
		close(fd);
		return -1;
	}

	if (listen(fd, BACK_LOG) == -1) {
		fprintf(stderr, "listen() failed: %s\n", strerror(errno));
		close(fd);
		return -1;
	}

	return fd;
}


int
main(int argc, char *argv[])
{
	int i, maxfd, nready, str_len;
	fd_set global_rfds, current_rfds;


	char buf[BUF_SIZE];
	struct timeval timeout;

	struct sockaddr_in cli_addr;
	socklen_t socklen;
	int socket_fd, connect_fd;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	socket_fd = create_socket(argv[1]);
	if (socket_fd == -1) {
		fprintf(stderr, "create_socket() failed.\n");
		exit(EXIT_FAILURE);
	}

	// 初始化select
	maxfd = socket_fd;

	FD_ZERO(&global_rfds);		//清空
	FD_SET(socket_fd, &global_rfds);	//将监听socket加入select检测的描述符集合

	printf("FD_SETSIZE = %d\n", FD_SETSIZE);

	socklen = sizeof(cli_addr);
	while (1) {

		// 等待5.5秒
		timeout.tv_sec = 5;	/* seconds */
		timeout.tv_usec = 500 * 1000;	/* microseconds 微秒 */

		current_rfds = global_rfds;

		nready = select(maxfd + 1, &current_rfds, NULL, NULL, &timeout);
		if (nready == -1) {
			fprintf(stderr, "select() failed\n");
			break;
		} else if (nready == 0) {
			printf("time out!\n");
			continue;
		}

		// 这种方式不太好
		for (i = 0; i < maxfd + 1; ++i) {
			if (FD_ISSET(i, &current_rfds)) {
				if (i == socket_fd) {
					// new client connection
					connect_fd = accept(socket_fd,
						(struct sockaddr *) &cli_addr,
						&socklen);
					FD_SET(connect_fd, &global_rfds);
					if (connect_fd > maxfd)
						maxfd = connect_fd;
					printf("new client connected\n");
				} else {
					// read msg
					str_len = read(i, buf, sizeof(buf));
					if (str_len <= 0) {
						// client closed
						FD_CLR(i, &global_rfds);
						close(i);
						printf("client closed\n");
					} else {
						// echo
						write(i, buf, str_len);
					}
				}
			}
		}
	}
	close(socket_fd);

	return 0;
}

