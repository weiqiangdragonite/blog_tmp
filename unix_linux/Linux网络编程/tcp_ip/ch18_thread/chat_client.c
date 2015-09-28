
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE	128
#define NAME_SIZE	20

void *send_msg(void *arg);
void *recv_msg(void *arg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr;
	pthread_t send_thread, recv_thread;
	void *thread_return;

	if (argc != 4) {
		fprintf(stderr, "Usage: %s <ip> <port> <name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	snprintf(name, sizeof(name), "[%s]", argv[3]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		perror("connect() failed");
		exit(EXIT_FAILURE);
	}

	pthread_create(&send_thread, NULL, send_msg, (void *) &sockfd);
	pthread_create(&recv_thread, NULL, recv_msg, (void *) &sockfd);

	pthread_join(send_thread, &thread_return);
	pthread_join(recv_thread, &thread_return);

	close(sockfd);

	return 0;
}

void *
send_msg(void *arg)
{
	int fd = *((int *) arg);
	char buf[NAME_SIZE + BUF_SIZE];

	while (1) {
		fgets(msg, sizeof(msg), stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
			close(fd);
			_exit(EXIT_SUCCESS);
		}

		snprintf(buf, sizeof(buf), "%s %s", name, msg);
		write(fd, buf, strlen(buf));
	}
	return NULL;
}

void *
recv_msg(void *arg)
{
	int fd = *((int *) arg);
	char buf[NAME_SIZE + BUF_SIZE];
	int str_len;

	while (1) {
		str_len = read(fd, buf, sizeof(buf) - 1);
		if (str_len <= 0)
			return (void *) -1;
		buf[str_len] = '\0';
		fputs(buf, stdout);
	}
	return NULL;
}




