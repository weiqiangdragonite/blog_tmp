#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>


#define UNIXDG_PATH	"/tmp/unix.dg"


void
dg_echo(int sockfd, struct sockaddr *cliaddr, socklen_t clilen)
{
	int n;
	socklen_t len;
	char msg[1024];

	while (1) {
		len = clilen;
		n = recvfrom(sockfd, msg, sizeof(msg), 0, cliaddr, &len);
		printf("bound name = %s\n", ((struct sockaddr_un *) cliaddr)->sun_path);

		sendto(sockfd, msg, n, 0, cliaddr, len);
	}
}


int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_un servaddr, cliaddr;

	sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);

	unlink(UNIXDG_PATH);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXDG_PATH);

	bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	dg_echo(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
}
