

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>




int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in mul_addr;
	struct ip_mreq join_addr;
	int str_len, ret;
	char buf[128];

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	memset(&mul_addr, 0, sizeof(mul_addr));
	mul_addr.sin_family = AF_INET;
	mul_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	mul_addr.sin_port = htons(atoi(argv[2]));

	ret = bind(sockfd, (struct sockaddr *) &mul_addr, sizeof(mul_addr));
	if (ret == -1) {
		perror("bind() failed");
		exit(EXIT_FAILURE);
	}

	// join multicast addr
	join_addr.imr_multiaddr.s_addr = inet_addr(argv[1]);
	join_addr.imr_interface.s_addr = htonl(INADDR_ANY);

	ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &join_addr, sizeof(join_addr));
	if (ret == -1) {
		perror("setsockopt() failed");
		exit(EXIT_FAILURE);
	}

	while (1) {
		str_len = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
		if (str_len < 0)
			break;
		buf[str_len] = '\0';
		printf("[recv] %s\n", buf);
	}
	close(sockfd);

	return 0;
}


