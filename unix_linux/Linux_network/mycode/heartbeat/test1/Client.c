/*
 * File: Client.c
 * Linux下一个socket心跳包的简单实现
 *
 * From: https://blog.csdn.net/freeking101/article/details/78922846
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>


#define BUF_SIZE	1024
#define BACK_LOG	128
#define SERV_PORT	10000


enum Type { HEART = 1, DATA };
struct PACKET_HEAD
{
	enum Type type;
	int length;
};


static void *send_heartbeat(void *arg)
{
	int count = 0;
	int fd = *((int *) arg);

	printf("The heartbeat sending thread started\n");

	while (1) {
		struct PACKET_HEAD head;
		head.type = HEART;
		head.length = 0;
		if (write(fd, &head, sizeof(head)) < 0) { // 要注意如果主线程close的话这里写时会报错
			fprintf(stderr, "thread: write error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		sleep(3);
		++count;
		if (count > 20)
			break;
	}

	printf("The heartbeat sending thread end\n");
}

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in svaddr;
	int ret;
	char msg[BUF_SIZE];
	

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <IP address>\n", argv[0]);
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		fprintf(stderr, "socket() failed: %s\n", strerror(errno));
		return -1;
	}

	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(SERV_PORT);
	ret = inet_pton(AF_INET, argv[1], &svaddr.sin_addr);
	if (ret != 1) {
		fprintf(stderr, "inet_pton() for %s failed: %s\n",
			argv[1], strerror(errno));
		close(sockfd);
		return -1;
	}

	ret = connect(sockfd, (struct sockaddr *) &svaddr, sizeof(svaddr));
	if (ret == -1) {
		fprintf(stderr, "connect() failed: %s\n", strerror(errno));
		close(sockfd);
		return -1;
	}
	printf("Connect to Server successfully.\n");


	/* 创建心跳发送线程 */
	pthread_t heartbeat_thread;

	if (pthread_create(&heartbeat_thread, NULL, (void *) send_heartbeat, (void *) &sockfd) != 0) {
		fprintf(stderr, "Create heartbeat_thread() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Create heartbeat_thread() success\n");


	sleep(5);
	while (1) {
		printf("\n>>> ");
		fgets(msg, sizeof(msg), stdin);  
		if (strncmp(msg, "exit", 4) == 0)
			break;

		struct PACKET_HEAD head;
		head.type = DATA;
		head.length = strlen(msg); // 包含\n 不含 \0
		
		if (write(sockfd, &head, sizeof(head)) < 0) {
			fprintf(stderr, "write error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (write(sockfd, &msg, head.length) < 0) {
			fprintf(stderr, "write error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		
	}
	close(sockfd);
	/* wait for return */
	pthread_join(heartbeat_thread, NULL);

	return 0;
}