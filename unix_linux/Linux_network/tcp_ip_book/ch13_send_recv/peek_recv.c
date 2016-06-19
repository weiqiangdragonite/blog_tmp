

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>


int servfd, sockfd;

int
main(int argc, char *argv[])
{
	struct sockaddr_in serv_addr, cli_addr;
	int str_len;
	socklen_t addr_len;
	char buf[1024];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	servfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	bind(servfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	listen(servfd, 5);

	addr_len = sizeof(cli_addr);
	sockfd = accept(servfd, (struct sockaddr *) &cli_addr, &addr_len);


	while (1) {
		str_len = recv(sockfd, buf, sizeof(buf) - 1, MSG_PEEK | MSG_DONTWAIT);
		if (str_len > 0)
			break;
	}

	buf[str_len] = '\0';
	printf("Buffering %d bytes: %s\n", str_len, buf);

	str_len = recv(sockfd, buf, sizeof(buf) - 1, 0);
	buf[str_len] = '\0';
	printf("Read again: %s\n", buf);

	close(sockfd);
	close(servfd);

	return 0;
}

