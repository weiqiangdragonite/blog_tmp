

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define BUF_SIZE	128
#define MAX_CLIENT	128

int client_counter = 0;
int client_fd[MAX_CLIENT];
pthread_mutex_t mutex;

void *handle_client(void *arg);
void send_msg(char *msg, int len);

int
main(int argc, char *argv[])
{
	int sockfd, clifd;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t addr_len;
	pthread_t tid;
	int i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	pthread_mutex_init(&mutex, NULL);
	memset(client_fd, -1, sizeof(client_fd));

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	listen(sockfd, 5);

	addr_len = sizeof(cli_addr);
	while (1) {
		clifd = accept(sockfd, (struct sockaddr *) &cli_addr, &addr_len);
		if (clifd == -1) {
			perror("accept() failed");
			continue;
		}

		// 这样写有效率问题，看书本的代码比较好
		pthread_mutex_lock(&mutex);
		for (i = 0; i < MAX_CLIENT; ++i) {
			if (client_fd[i] != -1)
				continue;
			client_fd[i] = clifd;
			++client_counter;
			break;
		}
		pthread_mutex_unlock(&mutex);

		pthread_create(&tid, NULL, handle_client, (void *) &clifd);
		pthread_detach(tid);
		printf("Connect client ip: %s\n", inet_ntoa(cli_addr.sin_addr));
	}
	close(sockfd);

	return 0;
}

void *
handle_client(void *arg)
{
	int fd = *((int *) arg);
	int str_len, i;
	char msg[BUF_SIZE];

	while ((str_len = read(fd, msg, sizeof(msg) - 1)) > 0) {
		msg[str_len] = '\0';
		send_msg(msg, str_len);
	}

	// remove disconnected client
	pthread_mutex_lock(&mutex);
	for (i = 0; i < MAX_CLIENT; ++i) {
		if (fd == client_fd[i]) {
			client_fd[i] = -1;
			break;
		}
	}
	--client_counter;
	pthread_mutex_unlock(&mutex);
	printf("client disconected\n");
	close(fd);

	return NULL;
}

void
send_msg(char *msg, int len)
{
	int i;
	pthread_mutex_lock(&mutex);

	for (i = 0; i < MAX_CLIENT; ++i) {
		if (client_fd[i] == -1)
			continue;
		write(client_fd[i], msg, len);
	}
	pthread_mutex_unlock(&mutex);
}


