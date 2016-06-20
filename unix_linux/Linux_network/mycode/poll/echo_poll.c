/*

#include <poll.h>

int poll(struct pollfd fds[], nfds_t nfds, int timeout);

Return number of ready file descriptors, 0 on timeout, or -1 on error.


struct pollfd {
	int fd;
	short events;	// request events
	short revents;	// returned events
};


存在问题，参考一下UNP 6.11节的程序

*/

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
#include <poll.h>


#define MAX_SIZE	128
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
	int i, fd, nready;


	char buf[BUF_SIZE];

	struct sockaddr_in cli_addr;
	socklen_t socklen;
	int socket_fd, connect_fd;
	ssize_t num_bytes;

	int timeout, maxi;
	struct pollfd client[MAX_SIZE];
	nfds_t maxfd;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	socket_fd = create_socket(argv[1]);
	if (socket_fd == -1) {
		fprintf(stderr, "create_socket() failed.\n");
		exit(EXIT_FAILURE);
	}

	memset(client, 0, sizeof(client));

	// 初始化client
	client[0].fd = socket_fd;
	client[0].events = POLLIN;	// 监听数据可读事件

	for (i = 1; i < MAX_SIZE; ++i)
		client[i].fd = -1;

	maxfd = socket_fd;
	maxi = -1;

	socklen = sizeof(cli_addr);
	while (1) {
		// -1是阻塞，0是不会阻塞，大于0至多阻塞timeout毫秒
		timeout = 1000;

		nready = poll(client, maxfd + 1, timeout);
		if (nready == -1) {
			fprintf(stderr, "poll() failed\n");
			break;
		} else if (nready == 0) {
			printf("time out!\n");
			continue;
		}

		/* new client connection */
		if (client[0].revents & POLLIN) {
			connect_fd = accept(socket_fd,
					(struct sockaddr *) &cli_addr,
					&socklen);

			for (i = 0; i < MAX_SIZE; ++i) {
				/* save connect client fd */
				if (client[i].fd == -1) {
					printf("Client connected, i = %d\n", i);
					client[i].fd = connect_fd;
					client[i].events = POLLIN;// 加入监听
					break;
				}
			}

			if (i == MAX_SIZE) {
				fprintf(stderr, "Too many clients\n");
				close(connect_fd);
				continue;
			}

			if (i > maxfd)
				maxfd = i;

			if (i > maxi)
				maxi = i;

			// no more readable descriptor
			if (--nready <= 0)
				continue;
		}

		// check all
		// i start from 1
		for (i = 1; i <= maxi; ++i) {
			if ((fd = client[i].fd) == -1)
				continue;

			if (client[i].revents & (POLLIN | POLLERR)) {
				memset(buf, 0, sizeof(buf));
				num_bytes = read(fd, buf, sizeof(buf));

				if (num_bytes < 0) {
					perror("read() failed");
					close(fd);
					client[i].fd = -1;
					continue;
				} else if (num_bytes == 0) {
					printf("client close the connection\n");
					close(fd);
					client[i].fd = -1;
					continue;
				}

				buf[num_bytes] = '\0'; // 截断最后一位设置为'\0'
				printf("recv [%d bytes]: %s\n", (int) num_bytes, buf);
				write(fd, buf, num_bytes); // echo
				// no more readable descriptor
				if (--nready <= 0)
					break;
			}
		}
	}
	close(socket_fd);

	return 0;
}







