

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



#define TTL	64

int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in mul_addr;
	int time_live = TTL;
	int fd, ret, str_len;
	char buf[10];

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
	mul_addr.sin_addr.s_addr = inet_addr(argv[1]);
	mul_addr.sin_port = htons(atoi(argv[2]));

	ret = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &time_live, sizeof(time_live));
	if (ret == -1) {
		perror("setsockopt() failed");
		exit(EXIT_FAILURE);
	}

	fd = open("news_sender.c", O_RDONLY);
	if (fd == -1) {
		perror("open() failed");
		exit(EXIT_FAILURE);
	}

	while ((str_len = read(fd, buf, sizeof(buf) - 1)) > 0) {
		buf[str_len] = '\0';
		sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &mul_addr, sizeof(mul_addr));
		sleep(3);
	}
	close(fd);
	close(sockfd);

	return 0;
}


