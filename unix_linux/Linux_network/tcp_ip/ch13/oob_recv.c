

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

void
sigurg_handler(int sig)
{
	int str_len;
	char buf[128];
	str_len = recv(sockfd, buf, sizeof(buf) - 1, MSG_OOB);
	buf[str_len] = '\0';
	printf("Urgent msg: %s\n", buf);
}

int
main(int argc, char *argv[])
{
	struct sockaddr_in serv_addr, cli_addr;
	int str_len;
	socklen_t addr_len;
	struct sigaction action;
	char buf[1024];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	action.sa_handler = sigurg_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGURG, &action, 0);

	servfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	bind(servfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	listen(servfd, 5);

	addr_len = sizeof(cli_addr);
	sockfd = accept(servfd, (struct sockaddr *) &cli_addr, &addr_len);

	fcntl(sockfd, F_SETOWN, getpid());

	while ((str_len = recv(sockfd, buf, sizeof(buf), 0)) != 0) {
		if (str_len < 0)
			continue;
		buf[str_len] = '\0';
		printf("[recv %d bytes] %s\n", str_len, buf);
	}

	close(sockfd);
	close(servfd);

	return 0;
}

