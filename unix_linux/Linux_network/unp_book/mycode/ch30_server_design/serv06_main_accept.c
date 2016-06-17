/*
 * 每个客户一个线程
 * 主线程调用accept
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
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>


#define MAXN		16384	/* max bytes client can request */
#define MAXLINE		1024


typedef struct {
	pthread_t thread_tid;
	long thread_count;	/* connection handle */
} Thread;

Thread *tptr;			/* array of Thread structures */

#define MAXNCLI		32
int clifd[MAXNCLI], iget, iput;
int nthreads;
pthread_mutex_t clifd_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clifd_cond = PTHREAD_COND_INITIALIZER;






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





void *
thread_main(void *arg)
{
	long i = (long) arg;
	int connfd;


	printf("thread %d starting\n", i);
	while (1) {

		pthread_mutex_lock(&clifd_mutex);
		while (iget == iput) {
			pthread_cond_wait(&clifd_cond, &clifd_mutex);
		} 

		connfd = clifd[iget];
		if (++iget == MAXNCLI)
			iget = 0;
		pthread_mutex_unlock(&clifd_mutex);

		tptr[i].thread_count += 1;

		web_child(connfd);
		close(connfd);
	}
}


void
thread_make(int i)
{
	long j = (long) i;
	if (pthread_create(&tptr[i].thread_tid, NULL, &thread_main, (void *) j) != 0) {
		fprintf(stderr, "pthread_create() failed\n");
		exit(-1);
	}
	return;
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
	void print_cpu_time(void);

	print_cpu_time();
	exit(0);
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
	int i, listenfd, connfd;
	socklen_t addrlen, clilen;
	struct sockaddr *cliaddr;


	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else {
		fprintf(stderr, "Usage: %s [host] <port> <nthreads>\n", argv[0]);
		exit(-1);
	}
	if (listenfd == -1) {
		fprintf(stderr, "tcp_listen() failed\n");
		exit(-1);
	}

	cliaddr = malloc(addrlen);
	if (cliaddr == NULL) {
		perror("malloc() failed");
		exit(-1);
	}

	nthreads = atoi(argv[argc - 1]);
	if ((tptr = calloc(nthreads, sizeof(Thread))) == NULL) {
		perror("calloc failed");
		exit(-1);
	}

	iget = iput = 0;

	for (i = 0; i < nthreads; ++i)
		thread_make(i);		/* only main thread returns */

	signal(SIGINT, sig_int);

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


		pthread_mutex_lock(&clifd_mutex);
		clifd[iput] = connfd;
		if (++iput == MAXNCLI)
			iput = 0;
		if (iput == iget) {
			fprintf(stderr, "iput == iget\n");
			exit(-1);
		}
		pthread_cond_signal(&clifd_cond);
		pthread_mutex_unlock(&clifd_mutex);
	}
	close(listenfd);

	return 0;
}








