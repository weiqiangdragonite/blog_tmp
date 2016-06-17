/*
 * 预先派生子进程(进程池)
 * 父进程向子进程传递描述符
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
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/select.h>


#define MAXN		16384	/* max bytes client can request */
#define MAXLINE		1024

#define max(a, b)	((a) > (b) ? (a) : (b))


typedef struct {
	pid_t child_pid;	/* process ID */
	int child_pipefd;	/* parent's stream pipe to/from child */
	int child_status;	/* 0 = ready */
	long child_count;	/* connection handled */
} Child;

Child *cptr;

static int nchildren;



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
		kill(cptr[i].child_pid, SIGTERM);
	/* wait for all children */
	while (wait(NULL) > 0)
		continue;
	if (errno != ECHILD)
		perror("wait() failed");

	print_cpu_time();
	for (i = 0; i < nchildren; ++i)
		printf("child %d counter: %ld\n", i, cptr[i].child_count);

	exit(0);
}




#define HAVE_MSGHDR_MSG_CONTROL		1
#define BUFSIZE				1024

ssize_t
read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
	struct msghdr msg;
	struct iovec iov[1];
	ssize_t n;

#ifdef HAVE_MSGHDR_MSG_CONTROL
	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
#else
	int newfd;

	msg.msg_accrights = (caddr_t) &newfd;
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if ((n = recvmsg(fd, &msg, 0)) <= 0)
		return n;


#ifdef HAVE_MSGHDR_MSG_CONTROL
	if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL
		&& cmptr->cmsg_len == CMSG_LEN(sizeof(int)))
	{
		if (cmptr->cmsg_level != SOL_SOCKET) {
			fprintf(stderr, "control level != SOL_SOCKET\n");
			exit(-1);
		}
		if (cmptr->cmsg_type != SCM_RIGHTS) {
			fprintf(stderr, "control type != SCM_RIGHTS\n");
			exit(-1);
		}

		*recvfd = *((int *) CMSG_DATA(cmptr));
	} else
		*recvfd = -1;	/* descriptor was not passed */
#else
	if (msg.msg_accrightslen == sizeof(int))
		*recvfd = newfd;
	else
		*recvfd = -1;	/* descriptor was not passed */
#endif

	return n;
}


ssize_t
write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
	struct msghdr msg;
	struct iovec iov[1];

#ifdef HAVE_MSGHDR_MSG_CONTROL
	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*((int *) CMSG_DATA(cmptr)) = sendfd;
#else
	msg.msg_accrights = (caddr_t) &sendfd;
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	return (sendmsg(fd, &msg, 0));
}


void
child_main(int i, int listenfd, int addrlen)
{
	char c;
	int connfd;
	ssize_t n;


	printf("child %ld starting\n", (long) getpid());
	while (1) {
		/* read descriptor from parent */
		if ((n = read_fd(STDERR_FILENO, &c, 1, &connfd)) == 0) {
			fprintf(stderr, "read_fd() return 0\n");
			continue;	/* continue or exit ? */
		}
		if (connfd < 0) {
			fprintf(stderr, "no descriptor from read_fd()\n");
			exit(-1);	/* continue or exit ? */
		}

		web_child(connfd);		/* process request */
		close(connfd);

		/* tell parent we're ready again */
		write(STDERR_FILENO, "", 1);
	}
}


pid_t
child_make(int i, int listenfd, int addrlen)
{
	pid_t pid;
	int sockfd[2];

	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd) == -1) {
		perror("socketpair() failed");
		exit(-1);
	}

	if ((pid = fork()) > 0) {
		close(sockfd[1]);
		cptr[i].child_pid = pid;
		cptr[i].child_pipefd = sockfd[0];
		cptr[i].child_status = 0;
		cptr[i].child_count = 0;
		return pid;	/* parent return */
	}

	/* childs */
	dup2(sockfd[1], STDERR_FILENO);
	close(sockfd[0]);
	close(sockfd[1]);
	close(listenfd);	/* does not need this open */

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
	int listenfd, i, navail, maxfd, nsel, connfd, rc;
	socklen_t addrlen, clilen;
	ssize_t n;
	fd_set rset, masterset;
	struct sockaddr *cliaddr;


	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else {
		fprintf(stderr, "Usage: %s [host] <port> <children>\n", argv[0]);
		exit(-1);
	}
	if (listenfd == -1)
		exit(-1);


	FD_ZERO(&masterset);
	FD_SET(listenfd, &masterset);
	maxfd = listenfd + 1;

	if ((cliaddr = malloc(addrlen)) == NULL) {
		perror("malloc() failed");
		exit(-1);
	}

	nchildren = atoi(argv[argc - 1]);
	navail = nchildren;
	cptr = calloc(nchildren, sizeof(Child));



	/* prefork all the children */
	for (i = 0; i < nchildren; ++i) {
		child_make(i, listenfd, addrlen);	/* parent returns */
		FD_SET(cptr[i].child_pipefd, &masterset);
		maxfd = max(maxfd, cptr[i].child_pipefd);
	}

	signal(SIGINT, sig_int);

	while (1) {
		rset = masterset;
		/* turn off if no available children */
		if (navail <= 0)
			FD_CLR(listenfd, &rset);

		nsel = select(maxfd + 1, &rset, NULL, NULL, NULL);

		/* check for new connections */
		if (FD_ISSET(listenfd, &rset)) {
			clilen = addrlen;
			connfd = accept(listenfd, cliaddr, &clilen);

			for (i = 0; i < nchildren; ++i) {
				if (cptr[i].child_status == 0)
					break;	/* find a ready child */
			}
			if (i == nchildren) {
				fprintf(stderr, "no available children\n");
				continue;	/* continue or exit ? */
			}

			cptr[i].child_status = 1;	/* mark child busy */
			cptr[i].child_count += 1;
			navail -= 1;

			/* send descriptor to child */
			n = write_fd(cptr[i].child_pipefd, "", 1, connfd);
			close(connfd);

			/* all done with select() results */
			if (--nsel == 0)
				continue;
		}

		/* find any newly-available children */
		for (i = 0; i < nchildren; ++i) {
			if (FD_ISSET(cptr[i].child_pipefd, &rset)) {
				if ((n = read(cptr[i].child_pipefd, &rc, 1)) == 0) {
					/* we should not exit and make a new child */
					fprintf(stderr, "child %ld terminated unexpectedly\n",
						cptr[i].child_pid);
					continue;
				}
				cptr[i].child_status = 0;
				navail += 1;

				/* all done with select() results */
				if (--nsel == 0)
					break;
			}
		}
	}

	return 0;
}





