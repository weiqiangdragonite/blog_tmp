#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define max(a, b)	((a) >= (b) ? (a) : (b))

#define BUF_SIZE	1024
#define SERV_PORT	10000
#define BACK_LOG	128


void sigchld_handler(int sig);
int catch_signal(int sig, void (*handler)(int));
void str_echo(int sockfd);


int
main(int argc, char *argv[])
{
	int listenfd, connfd, udpfd;
	int nready, maxfd;
	char msg[BUF_SIZE];
	pid_t childpid;
	fd_set rset;
	ssize_t n;
	socklen_t len;
	const int opt = 1;
	struct sockaddr_in cliaddr, svaddr;

	/* create listening tcp socket */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svaddr.sin_port = htons(SERV_PORT);

	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	bind(listenfd, (struct sockaddr *) &svaddr, sizeof(svaddr));
	listen(listenfd, BACK_LOG);


	/* create udp socket */
	udpfd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svaddr.sin_port = htons(SERV_PORT);

	bind(udpfd, (struct sockaddr *) &svaddr, sizeof(svaddr));

	/* must call waitpid() */
	catch_signal(SIGCHLD, sigchld_handler);

	FD_ZERO(&rset);
	maxfd = max(listenfd, udpfd) + 1;
	printf("maxfd = %d, listenfd = %d, udpfd = %d\n", maxfd, listenfd, udpfd);
	while (1) {
		FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);

		if ((nready = select(maxfd, &rset, NULL, NULL, NULL)) == -1) {
			if (errno == EINTR)
				continue;	/* back to while */
			
			fprintf(stderr, "select() failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		/* for tcp */
		if (FD_ISSET(listenfd, &rset)) {
			len = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);

			if ((childpid = fork()) == 0) {
				close(listenfd);
				str_echo(connfd);
				exit(EXIT_SUCCESS);
			}
			close(connfd);
		}

		/* for udp */
		if (FD_ISSET(udpfd, &rset)) {
			len = sizeof(cliaddr);
			n = recvfrom(udpfd, msg, BUF_SIZE, 0, (struct sockaddr *) &cliaddr, &len);

			sendto(udpfd, msg, n, 0, (struct sockaddr *) &cliaddr, len);
		}
	}

	return 0;
}


void
sigchld_handler(int sig)
{
	pid_t pid;
	int stat;

	/* use waitpid() to wait for all the child */
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}

/*
 * 信号处理
 */
int
catch_signal(int sig, void (*handler)(int))
{
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	return sigaction(sig, &action, NULL);
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



