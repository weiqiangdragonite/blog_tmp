#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>


#define UNIXDG_PATH	"/tmp/unix.dg"
#define MAXLINE		1024


void
dg_cli(FILE *fp, int sockfd, struct sockaddr *servaddr, socklen_t servlen)
{
	int n;
	char sendline[MAXLINE];
	char recvline[MAXLINE + 1];

	while (fgets(sendline, MAXLINE, fp) != NULL) {
		sendto(sockfd, sendline, strlen(sendline), 0, servaddr, servlen);

		n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
		recvline[n] = '\0';
		fputs(recvline, stdout);
	}
}


int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_un cliaddr, servaddr;

	sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);

	/* important !!! bind an address for us */
	memset(&cliaddr, 0, sizeof(cliaddr));
	cliaddr.sun_family = AF_LOCAL;
	strcpy(cliaddr.sun_path, tmpnam(NULL));

	bind(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr));


	/* fill in server address */
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXDG_PATH);

	dg_cli(stdin, sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	return 0;
}


