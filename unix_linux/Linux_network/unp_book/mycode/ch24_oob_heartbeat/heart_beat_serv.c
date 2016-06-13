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
static int maxnalarms;	/* alarms w/no client probe before quit */
static int nprobes;	/* alarms since last client response */

static void sig_urg(int sig);
static void sig_alrm(int sig);
static void sig_chld(int sig);

void heart_beat_serv(int servfd_arg, int nsec_arg, int maxnalarms_arg);
void str_echo(int sockfd);


static void
sig_chld(int sig)
{
	pid_t pid;
	int stat;

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}


int
main(int argc, char *argv[])
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t socklen;
	struct sockaddr_in svaddr, cliaddr;
	int ret;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(-1);
	}

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		fprintf(stderr, "socket() failed: %s\n", strerror(errno));
		return -1;
	}

	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svaddr.sin_port = htons(atoi(argv[1]));

	ret = bind(listenfd, (struct sockaddr *) &svaddr, sizeof(svaddr));
	if (ret == -1) {
		fprintf(stderr, "bind() failed: %s\n", strerror(errno));
		close(listenfd);
		return -1;
	}

#define BACK_LOG	1024
	ret = listen(listenfd, BACK_LOG);
	if (ret == -1) {
		fprintf(stderr, "listen() failed: %s\n", strerror(errno));
		close(listenfd);
		return -1;
	}


	/* 处理子进程 */
	signal(SIGCHLD, sig_chld);


	while (1) {
		socklen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &socklen);
		if (connfd == -1) {
			if (errno == EINTR)
				continue;
			fprintf(stderr, "accept() failed: %s\n", strerror(errno));
			exit(-1);
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
str_echo(int sockfd)
{
	ssize_t n;
	char buf[1024];


	heart_beat_serv(sockfd, 2, 5);


again:
	while ((n = read(sockfd, buf, sizeof(buf))) > 0)
		writen(sockfd, buf, n);

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0) {
		perror("read() failed");
		exit(-1);
	}
}


/*
 * nsec: 轮询频率
 * maxnalarms: 放弃连接前持续无响应次数
 */
void
heart_beat_serv(int servfd_arg, int nsec_arg, int maxnalarms_arg)
{
	/* set globals for signal handlers */
	servfd = servfd_arg;
	if ((nsec == nsec_arg) < 1)
		nsec = 1;
	if ((maxnalarms = maxnalarms_arg) < nsec)
		maxnalarms = nsec;
	nprobes = 0;


	signal(SIGURG, sig_urg);
	fcntl(servfd, F_SETOWN, getpid());

	signal(SIGALRM, sig_alrm);
	alarm(nsec);
}


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
	send(servfd, &c, 1, MSG_OOB);	/* echo back out-of-band byte */

	nprobes = 0;	/* reset counter */
	return;		/* may interrupt server code */
}


static void
sig_alrm(int sig)
{
	if (++nprobes > maxnalarms) {
		fprintf(stderr, "no probes from client\n");
		exit(EXIT_FAILURE);
	}

	alarm(nsec);
	return;		/* may interrupt server code */
}





