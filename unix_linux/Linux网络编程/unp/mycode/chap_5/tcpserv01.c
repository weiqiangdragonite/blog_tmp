/*
 * 回射服务器
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#define SERV_PORT	10000
#define BACK_LOG	128
#define BUF_SIZE	100

void str_echo(int sockfd);

int
main(int argc, char *argv[])
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t socklen;
	struct sockaddr_in svaddr, cliaddr;
	int ret;

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

	socklen = sizeof(cliaddr);
	while (1) {
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &socklen);
		if (connfd == -1) {
			fprintf(stderr, "accept() failed: %s\n", strerror(errno));
			continue;
		}

		childpid = fork();
		switch (childpid) {
		case -1:	/* error */
			fprintf(stderr, "fock() failed: %s\n", strerror(errno));
			break;
		case 0:		/* child */
			close(listenfd);
			str_echo(connfd);
			close(connfd);
			exit(0);
		default:	/* parent */
			close(connfd);
			break;
		}
	}
	close(listenfd);
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

