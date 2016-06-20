/*
 * 回射服务器
 * 使用poll
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <poll.h>


#include <arpa/inet.h>
#include <sys/socket.h>
#include <limits.h>

#define OPEN_MAX	1024
#define INFTIM		-1


#define SERV_PORT	10000
#define BACK_LOG	128
#define BUF_SIZE	100

void str_echo(int sockfd);


int
main(int argc, char *argv[])
{
	int listenfd, connfd, sockfd;
	socklen_t socklen;
	struct sockaddr_in svaddr, cliaddr;
	int ret, i, nready;
	ssize_t n;
	char buf[BUF_SIZE];

	nfds_t maxi;
	struct pollfd client[OPEN_MAX];


	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		fprintf(stderr, "socket() failed: %s\n", strerror(errno));
		return -1;
	}

	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svaddr.sin_port = htons(SERV_PORT);

	ret = bind(listenfd, (struct sockaddr *) &svaddr, sizeof(svaddr));
	if (ret == -1) {
		fprintf(stderr, "bind() failed: %s\n", strerror(errno));
		close(listenfd);
		return -1;
	}

	ret = listen(listenfd, BACK_LOG);
	if (ret == -1) {
		fprintf(stderr, "listen() failed: %s\n", strerror(errno));
		close(listenfd);
		return -1;
	}





	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;	/* 普通数据可读 */
	for (i = 1; i < OPEN_MAX; ++i)
		client[i].fd = -1;
	maxi = 0;			/* max index into client[] array */



	while (1) {
		// -1是阻塞，0是不会阻塞，大于0至多阻塞timeout毫秒
		nready = poll(client, maxi + 1, INFTIM);


		/* new client connection */
		if (client[0].revents & POLLRDNORM) {
			socklen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &socklen);

			/* 从下标1开始，0固定用于监听套接字 */
			for (i = 1; i < OPEN_MAX; ++i) {
				if (client[i].fd < 0) {
					client[i].fd = connfd;
					break;
				}
			}

			if (i == OPEN_MAX) {
				fprintf(stderr, "Too many clients\n");
				exit(EXIT_FAILURE);
			}

			client[i].events = POLLRDNORM;
			if (i > maxi)
				maxi = i;

			/* no more readable descriptor */
			if (--nready <= 0)
				continue;
		}


		/* check all clients for data */
		for (i = 1; i <= maxi; ++i) {
			if ((sockfd = client[i].fd) < 0)
				continue;

			if (client[i].revents & (POLLRDNORM | POLLERR)) {
				if ((n = read(sockfd, buf, BUF_SIZE)) < 0) {
					if (errno == ECONNRESET) {
						close(sockfd);
						client[i].fd = -1;
					} else {
						fprintf(stderr, "read() failed: %s\n", strerror(errno));
						exit(EXIT_FAILURE);
					}
				} else if (n == 0) {
					/* connection closed by client */
					close(sockfd);
					client[i].fd = -1;
				} else {
					write(sockfd, buf, n);
				}

				/* no more readable descriptor */
				if (--nready <= 0)
					break;
			}
		}
	}
	return 0;
}




void
str_echo(int sockfd)
{
	ssize_t n;
	char buf[BUF_SIZE];

	while (1) {
		n = read(sockfd, buf, BUF_SIZE);
		if (n > 0) {
			if (write(sockfd, buf, n) != n) {
				fprintf(stderr, "str_echo() - write() failed: %s\n",
					strerror(errno));
				break;
			}
		} else if (n < 0 && errno == EINTR) {
			/* EINTR: 由于信号中断，没读到任何数据 */
			continue;
		} else if (n < 0) {
			fprintf(stderr, "str_echo() - read() failed: %s\n",
				strerror(errno));
			break;
		} else if (n == 0) {
			/* end */
			break;
		}
	}
}

