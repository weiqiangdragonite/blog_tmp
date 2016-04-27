
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>


#define MAXBUF	1024




int
main(int argc, char *argv[])
{


	int sockfd[65536];
	int i;
	struct sockaddr_in svaddr;
	char buffer[MAXBUF];

	if (argc != 3) {
		fprintf(stderr, "Uasge: %s <ip> <port>\n", argv[0]);
		exit(-1);
	}



	/* 2 - 初始化服务器端（对方）的地址和端口信息 */
	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(atoi(argv[2]));
	if (inet_pton(AF_INET, argv[1], &(svaddr.sin_addr)) != 1) {
		fprintf(stderr, "inet_pton() convert %s failed: %s\n",
			argv[1], strerror(errno));
		exit(errno);
	}


	i = 0;
	while (1) {
		/* 1 - 创建一个 socket 用于 tcp 通信 */
		if ((sockfd[i] = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("socket() failed");
			exit(errno);
		}


		/* 3 - 连接服务器 */
		if (connect(sockfd[i], (struct sockaddr *) &svaddr,
			sizeof(svaddr)) != 0)
		{
			perror("connect() failed");
			exit(errno);
		}


		/* 连接后什么也不做，继续生产下一个socket */
		printf("client %d connected\n", ++i);
	}

	return 0;
}


