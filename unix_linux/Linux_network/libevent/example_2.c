/*
 * Example: Forking ROT13 server
 *
 * Here’s another example program. It is a trivial server that listens for TCP
 * connections on port 40713, reads data from its input one line at a time,
 * and writes out the ROT13 obfuscation of line each as it arrives. It uses
 * the Unix fork() call to create a new process for each incoming connection.
 *
 * Original example from Libevent Documentation Book: Programming with Libevent.
 *
 * rewrite by <weiqiangdragonite@gmail.com>
 * update on 2015/10/08
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_LINE 16384
#define PORT 40713
#define BAK_LOG 10


char
rot13_char(char c)
{
	if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
		return c + 13;
	else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
		return c - 13;
	else
		return c;
}


void
children(int fd)
{
	char outbuf[MAX_LINE + 1];
	size_t outbuf_used = 0;
	ssize_t result;

	while (1) {
		char ch;
		result = read(fd, &ch, 1);
		if (result == 0) {
			break;
		} else if (result == -1) {
			perror("read() failed");
			break;
		}

		/* keep the user from overflowing the buffer */
		if (outbuf_used < sizeof(outbuf))
			outbuf[outbuf_used++] = rot13_char(ch);

		if (ch == '\n') {
			write(fd, outbuf, outbuf_used);
			outbuf_used = 0;
			continue;
		}
	}
}


void
run(void)
{
	int sockfd, clifd, val;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t len;

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	val = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		perror("bind() failed");
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, BAK_LOG) == -1) {
		perror("listen() failed");
		exit(EXIT_FAILURE);
	}

	len = sizeof(cli_addr);
	while (1) {
		clifd = accept(sockfd, (struct sockaddr *) &cli_addr, &len);
		if (clifd == -1) {
			perror("accept() failed");
			continue;
		}

		switch (fork()) {
		case -1:
			perror("fork() failed");
			close(clifd);
			continue;
		case 0:
			close(sockfd);
			children(clifd);
			close(clifd);
			_exit(EXIT_SUCCESS);
		default:
			close(clifd);
		}
	}
}


int
main(int argc, char *argv[])
{
	run();
	return 0;
}


