/*
 * 预先派生子进程(进程池)
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>


#define MAXN		16384	/* max bytes client can request */
#define MAXLINE		1024


static int nchildren;
static pid_t *pids;
long *cptr, *meter(int);


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


/*
 * this code is combine tlpi and unp
 *
 * PAINFULLY SLOW VERSION !!!
 */
ssize_t
readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t n, num_read;
	char c;
	char *p;

	p = ptr;
	for (n = 1; n < maxlen; ++n) {
		num_read = read(fd, &c, 1);

		if (num_read == -1) {
			if (errno == EINTR)	/* interrupt, restart */
				continue;
			else
				return -1;
		} else if (num_read == 0) {
			*p = '\0';		/* EOF, (n - 1) bytes return */
			return (n - 1);
		} else {
			*p++ = c;		/* read to maxlen - 1 bytes */
			if (c == '\n')
				break;
		}
	}

	*p = '\0';
	return n;
}



void
web_child(int sockfd)
{
	int ntowrite;
	ssize_t nread;
	char line[MAXLINE], result[MAXN];

	while (1) {
		if ((nread = readline(sockfd, line, MAXLINE)) == 0)
			return;		/* connection closed by other end */

		/* line from client specifies bytes to write back */
		ntowrite = atol(line);
		if ((ntowrite <= 0) || (ntowrite > MAXN)) {
			fprintf(stderr, "client request for %d bytes\n",
				ntowrite);
			return;
		}

		writen(sockfd, result, ntowrite);
	}
}


void
print_cpu_time(void)
{
	double user, sys;
	struct rusage myusage, childusage;

	if (getrusage(RUSAGE_SELF, &myusage) < 0) {
		perror("getrusage() parent failed");
		exit(-1);
	}
	if (getrusage(RUSAGE_CHILDREN, &childusage) < 0) {
		perror("getrusage() children failed");
		exit(-1);
	}


	user = (double) myusage.ru_utime.tv_sec +
			myusage.ru_utime.tv_usec / 1000000.0;
	user += (double) childusage.ru_utime.tv_sec +
			childusage.ru_utime.tv_usec / 1000000.0;

	sys = (double) myusage.ru_stime.tv_sec +
			myusage.ru_stime.tv_usec / 1000000.0;
	sys += (double) childusage.ru_stime.tv_sec +
			childusage.ru_stime.tv_usec / 1000000.0;

	printf("\nuser time = %g, sys time = %g\n", user, sys);
}




void
sig_int(int signo)
{
	int i;
	void print_cpu_time(void);

	/* terminate all children */
	for (i = 0; i < nchildren; ++i)
		kill(pids[i], SIGTERM);
	/* wait for all children */
	while (wait(NULL) > 0)
		continue;
	if (errno != ECHILD)
		perror("wait() failed");

	print_cpu_time();
	for (i = 0; i < nchildren; ++i)
		printf("child %d counter: %ld\n", i, cptr[i]);

	exit(0);
}

void
child_main(int i, int listenfd, int addrlen)
{
	int connfd;
	socklen_t clilen;
	struct sockaddr *cliaddr;

	cliaddr = malloc(addrlen);
	if (cliaddr == NULL) {
		perror("malloc() failed");
		exit(-1);
	}

	printf("child %ld starting\n", (long) getpid());
	while (1) {
		clilen = addrlen;
		if ((connfd = accept(listenfd, cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;
			else {
				perror("accept() error");
				exit(-1);
			}
		}
		cptr[i] += 1;

		web_child(connfd);		/* process request */
		close(connfd);
	}
}


pid_t
child_make(int i, int listenfd, int addrlen)
{
	pid_t pid;

	if ((pid = fork()) > 0)
		return pid;	/* parent return */

	child_main(i, listenfd, addrlen);	/* child never returns */
}



/*
 * for tcp server: create tcp socket, bind to server, listen for request
 * success return socket fd, otherwise return -1
 */
int
tcp_listen(const char *hostname, const char *service, socklen_t *len)
{
	int listenfd, n;
	const int on = 1;
	struct addrinfo hints, *res, *rp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(hostname, service, &hints, &res)) != 0) {
		fprintf(stderr, "tcp_listen - getaddrinfo() failed: %s\n",
			gai_strerror(n));
		return -1;
	}

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		listenfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		/* If error, try next address */
		if (listenfd == -1)
			continue;

		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on,
			sizeof(on)) == -1)
		{
			fprintf(stderr, "tcp_listen - setsockopt() failed: %s\n",
				strerror(errno)); 
			close(listenfd);
			freeaddrinfo(res);
			return -1;
		}


		if (bind(listenfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;	/* success */

		/* bind failed: close this socket and try next address */
		close(listenfd);
	}

	/* error */
	if (rp == NULL) {
		fprintf(stderr, "tcp_listen failed\n");
		listenfd = -1;
	}


	if (listenfd != -1) {
#ifndef BACK_LOG
#define BACK_LOG 1024
#endif
		if (listen(listenfd, BACK_LOG) == -1) {
			fprintf(stderr, "tcp_listen - listen() failed: %s\n",
				strerror(errno));
			close(listenfd);
			listenfd = -1;
		}
	}

	if (len != NULL)
		*len = res->ai_addrlen;	/* return sizeof protocol address */

	freeaddrinfo(res);
	return listenfd;
}



int
main(int argc, char *argv[])
{
	int listenfd, i;
	socklen_t addrlen;

	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else {
		fprintf(stderr, "Usage: %s [host] <port> <children>\n", argv[0]);
		exit(-1);
	}

	nchildren = atoi(argv[argc - 1]);
	pids = calloc(nchildren, sizeof(pid_t));
	if (pids == NULL) {
		perror("calloc() failed");
		exit(-1);
	}

	cptr = meter(nchildren);

	for (i = 0; i < nchildren; ++i)
		pids[i] = child_make(i, listenfd, addrlen);
	/* parent returns */

	signal(SIGINT, sig_int);

	while (1) {
		pause();	/* everything done by children */
	}

	return 0;
}



long *
meter(int nchildren)
{
	int fd;
	long *ptr;

#ifdef MAP_ANON
	ptr = mmap(0, nchildren * sizeof(long), PROT_READ | PROT_WRITE,
		MAP_ANON | MAP_SHARED, -1, 0);
#else
	fd = open("/dev/zero", O_RDWR, 0);
	ptr = mmap(0, nchildren * sizeof(long), PROT_READ | PROT_WRITE,
		MAP_SHARED, fd, 0);
	close(fd);
#endif
	return ptr;
}


