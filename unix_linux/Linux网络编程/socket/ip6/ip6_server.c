/*
 * use ipv6 for a test
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>


int
main(int argc, char *argv[])
{
	struct sockaddr_in6 serv_addr, cli_addr;
	int sockfd, clifd;
	socklen_t addr_len;
	int str_len;
	char buf[1024];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin6_family = AF_INET6;
	serv_addr.sin6_addr = in6addr_any;
	serv_addr.sin6_port = htons(atoi(argv[1]));

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		perror("bind() failed");
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, 5) == -1) {
		perror("listen() failed");
		exit(EXIT_FAILURE);
	}

	addr_len = sizeof(cli_addr);
	while (1) {
		clifd = accept(sockfd, (struct sockaddr *) &cli_addr, &addr_len);
		if (clifd == -1) {
			perror("accept() failed");
			continue;
		}

		char str[INET6_ADDRSTRLEN];
		if (inet_ntop(AF_INET6, &cli_addr.sin6_addr, str, sizeof(str)) == NULL) {
			fprintf(stderr, "inet_ntop() failed\n");
		} else {
			printf("client connect, ip = %s, port = %d\n",
			str, cli_addr.sin6_port);
		}

		str_len = read(clifd, buf, sizeof(buf) - 1);
		buf[str_len] = '\0';
		write(clifd, buf, str_len);

		close(clifd);
	}
	close(sockfd);

	return 0;
}


