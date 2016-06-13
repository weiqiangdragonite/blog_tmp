/*
 *
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>


#define UNIXSTR_PATH	"/tmp/unix.str"

void
sig_chld(int sig)
{
	pid_t pid;
	int stat;

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
}


void
str_echo(int sockfd)
{
	ssize_t n;
	char buf[1024];

again:
	while ((n = read(sockfd, buf, sizeof(buf))) > 0)
		write(sockfd, buf, n);

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		perror("read() faileds");
}


int
main(int argc, char *argv[])
{
	int listenfd, connfd;
	socklen_t clilen;
	struct sockaddr_un cliaddr, servaddr;

	listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);

	unlink(UNIXSTR_PATH);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXSTR_PATH);

	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	listen(listenfd, 128);

	signal(SIGCHLD, sig_chld);

	while (1) {
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;
			else {
				perror("accept() failed");
				exit(-1);
			}
		}

		if (fork() == 0) {
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}

		close(connfd);
	}
	close(listenfd);
	return 0;
}


