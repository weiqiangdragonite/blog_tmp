/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/socket.h>



static int servfd;
static int nsec;	/* seconds between each alarm */
static int maxnprobes;	/* probes w/no response before quit */
static int nprobes;	/* probes since last server response */

static void sig_urg(int sig);
static void sig_alrm(int sig);

void heart_beat_cli(int servfd_arg, int nsec_arg, int maxnprobes_arg);
void str_cli(FILE *fp, int sockfd);


int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

	connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

	str_cli(stdin, sockfd);

	return 0;
}


/*
 * write n bytes to fd
 * no buffer
 */
ssize_t
writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *p;

	p = (const char *) vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, p, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;	/* call write() again */
			else
				return -1;
		}

		nleft -= nwritten;
		p += nwritten;
	}

	return n;
}

void
str_cli(FILE *fp, int sockfd)
{
	int maxfd, stdineof;
	fd_set rset;
	char buf[1024];
	ssize_t n;

	stdineof = 0;
	FD_ZERO(&rset);



	heart_beat_cli(sockfd, 2, 5);



#define MAX(a, b)	((a) > (b) ? (a) : (b))

	while (1) {
		if (stdineof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);

		maxfd = MAX(sockfd, fileno(fp)) + 1;

		n = select(maxfd, &rset, NULL, NULL, NULL);
		if (n == -1) {
			if (errno == EINTR)
				continue;
			else
				perror("select() failed");
			exit(EXIT_FAILURE);
		}

		/* socket is readable */
		if (FD_ISSET(sockfd, &rset)) {
			if ((n = read(sockfd, buf, sizeof(buf))) == 0) {
				if (stdineof == 1)
					return;	/* normal termination */
				else
					fprintf(stderr, "server terminated prematurely\n");
				return;
			} else if (n < 0) {
				/* 这里没有处理中断问题，用readn()应该不行 */
				perror("read() from socket failed");
				return;
			}

			writen(fileno(stdout), buf, n);
		}

		/* input is readable */
		if (FD_ISSET(fileno(fp), &rset)) {
			if ((n = read(fileno(fp), buf, sizeof(buf))) == 0) {
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);	/* sebd FIN */
				FD_CLR(fileno(fp), &rset);
				continue;
			} else if (n < 0) {
				/* 这里没有处理中断问题，用readn()应该不行 */
				perror("read() from stdin failed");
				return;
			}

			/* write n bytes to socket */
			writen(sockfd, buf, n);
		}
	}
}


/*
 * nsec: 轮询频率
 * maxnprobes: 放弃连接前持续无响应次数
 * nprobes: 无响应次数计数器
 */
void
heart_beat_cli(int servfd_arg, int nsec_arg, int maxnprobes_arg)
{
	/* set globals for signal handlers */
	servfd = servfd_arg;

	if ((nsec == nsec_arg) < 1)
		nsec = 1;
	if ((maxnprobes = maxnprobes_arg) < nsec)
		maxnprobes = nsec;
	nprobes = 0;

	/* 心跳机制有两个信号: SIGALRM, SIGURG */

	signal(SIGURG, sig_urg);
	fcntl(servfd, F_SETOWN, getpid());	/* set owner */

	signal(SIGALRM, sig_alrm);
	alarm(nsec);
}


/*
 * 接收服务器发过来的心跳包
 */
static void
sig_urg(int sig)
{
	int n;
	char c;

	if ((n = recv(servfd, &c, 1, MSG_OOB)) < 0) {
		if (errno != EWOULDBLOCK) {
			fprintf(stderr, "recv() failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	nprobes = 0;	/* reset counter */
	return;		/* may interrupt client code */
}


/*
 * 发送心跳包给服务器
 */
static void
sig_alrm(int sig)
{
	if (++nprobes > maxnprobes) {
		fprintf(stderr, "server is unreachable\n");
		exit(EXIT_FAILURE);
	}

	send(servfd, "1", 1, MSG_OOB);
	alarm(nsec);
	return;		/* may interrupt client code */
}





