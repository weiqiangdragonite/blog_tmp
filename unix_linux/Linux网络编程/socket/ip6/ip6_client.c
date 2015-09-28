/*
 * use ipv6 for a test
 *
 * ./client ::1/128 10000
 *
 * 貌似同一台机下，用ipv6的地址比如 fe80::a00:27ff:fe92:2599
 * 这个的话，好像不能connect：
 * connect() failed: Invalid argument
 *
 * 不知什么原因呢，如果用两台机子的话会怎样呢
 *
 * 用上面的 ::1/128 就没问题，这个应该是回环的ipv6地址吧
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
	struct sockaddr_in6 serv_addr;
	int sockfd;
	int str_len, ret;
	char msg[] = "hello, world";
	char buf[1024];

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <ip6> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin6_family = AF_INET6;
	serv_addr.sin6_port = htons(atoi(argv[2]));

	ret = inet_pton(AF_INET6, argv[1], &serv_addr.sin6_addr);
	// 如果ip6地址是 ::1/128 这个的话，ret会等于0
	if (ret <= 0) {
		if (ret == 0)
			fprintf(stderr, "Not in presentation format\n");
		else
			perror("inet_pton() failed");
		exit(EXIT_FAILURE);
	}

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		perror("connect() failed");
		exit(EXIT_FAILURE);
	}

	write(sockfd, msg, strlen(msg));
	str_len = read(sockfd, buf, sizeof(buf) - 1);
	buf[str_len] = '\0';
	printf("[recv %d bytes] %s\n", str_len, buf);

	close(sockfd);

	return 0;
}


