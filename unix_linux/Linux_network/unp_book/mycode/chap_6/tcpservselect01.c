/*
 * 回射服务器
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>

#include <arpa/inet.h>
#include <sys/socket.h>

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
	int ret, i;
	ssize_t n;
	int maxi, maxfd, nready, client[FD_SETSIZE];
	fd_set rset, allset;
	char buf[BUF_SIZE];


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





	maxfd = listenfd;
	maxi = -1;
	for (i = 0; i < FD_SETSIZE; ++i)
		client[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);


	socklen = sizeof(cliaddr);
	while (1) {
		rset = allset;
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

		/* new client connection */
		if (FD_ISSET(listenfd, &rset)) {
			connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &socklen);

			for (i = 0; i < FD_SETSIZE; ++i) {
				if (client[i] < 0) {
					client[i] = connfd;
					break;
				}
			}

			if (i == FD_SETSIZE) {
				fprintf(stderr, "Too many clients\n");
				exit(EXIT_FAILURE);
			}

			/* add new descriptor to set */
			FD_SET(connfd, &allset);
			if (connfd > maxfd)
				maxfd = connfd;
			if (i > maxi)
				maxi = i;

			/* no more readable descriptor */
			if (--nready <= 0)
				continue;
		}

		/* check all clients for data */
		for (i = 0; i <= maxi; ++i) {
			if ((sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				if ((n = read(sockfd, buf, BUF_SIZE)) == 0) {
					/* client close connection */
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
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

