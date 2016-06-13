#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>


#define UNIXSTR_PATH	"/tmp/unix.str"
#define MAXLINE		1024

void
str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];

	while (fgets(sendline, MAXLINE, fp) != NULL) {
		write(sockfd, sendline, strlen(sendline));

		if (read(sockfd, recvline, MAXLINE) <= 0)
			break;

		fputs(recvline, stdout);
	}
}


int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_un servaddr;

	sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXSTR_PATH);

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	str_cli(stdin, sockfd);

	return 0;
}




