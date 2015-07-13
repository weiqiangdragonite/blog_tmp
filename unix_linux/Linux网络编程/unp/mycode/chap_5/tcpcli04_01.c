/*
 * 回射客户端
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#define SERV_PORT	10000
#define BUF_SIZE	100

void str_cli(int sockfd);

int
main(int argc, char *argv[])
{
	int sockfd[5];
	struct sockaddr_in svaddr;
	int ret, i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <IP address>\n", argv[0]);
		return -1;
	}

	for (i = 0; i < 5; ++i) {
		sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);

		memset(&svaddr, 0, sizeof(svaddr));
		svaddr.sin_family = AF_INET;
		svaddr.sin_port = htons(SERV_PORT);
		ret = inet_pton(AF_INET, argv[1], &svaddr.sin_addr);
		if (ret != 1) {
			fprintf(stderr, "inet_pton() for %s failed: %s\n",
				argv[1], strerror(errno));
			close(sockfd[i]);
			return -1;
		}

		ret = connect(sockfd[i], (struct sockaddr *) &svaddr, sizeof(svaddr));
		if (ret == -1) {
			fprintf(stderr, "connect() failed: %s\n", strerror(errno));
			close(sockfd[i]);
			return -1;
		}
	}

	str_cli(sockfd[0]);


	return 0;
}

/*
 * 当输入超出BUF_SIZE时，自己试一下
 */
void
str_cli(int sockfd)
{
	char recvbuf[BUF_SIZE], sendbuf[BUF_SIZE];
	ssize_t n;

	while (1) {
		printf("\n(client) >>> ");
		if (fgets(sendbuf, BUF_SIZE, stdin) == NULL)
			break;
		//printf("fgets() n = %ld\n", strlen(sendbuf));

		n = write(sockfd, sendbuf, 1);
		sleep(1);
		n = write(sockfd, sendbuf + 1, strlen(sendbuf) - 1);
		

		n = read(sockfd, recvbuf, BUF_SIZE);
		//printf("read() n = %ld\n", n);
		recvbuf[n] = '\0';
		if (n == 0) {
			/* end */
			fprintf(stderr, "str_cli() - read() end: server terminated.\n");
			break;
		} else if (n > 0) {
			printf("(server) >>> ");
			fputs(recvbuf, stdout);
		}
	}
}

