/*
 * http://zhoulifa.bokee.com/6065676.html
 *
 * Filename: simple-udpserver.c
 * Purpose: 基本编程步骤说明，演示了UDP编程的服务器端编程步骤
 * Wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
 * Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
 * Date time: 2007-01-24 20:22:00
 * Note: 任何人可以任意复制代码并运用这些文档，当然包括你的商业用途
 * 但请遵循GPL
 * Hope: 希望越来越多的人贡献自己的力量，为科学技术发展出力
 * 科技站在巨人的肩膀上进步更快！感谢有开源前辈的贡献！
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE	1024


int
main(int argc, char *argv[])
{
	int sockfd;
	int nbytes;
	struct sockaddr_in svaddr, cliaddr;
	socklen_t addr_len;
	char buf[BUFSIZE];
	char cli_ip[INET_ADDRSTRLEN];

	if (argc != 2 && argc != 3) {
		fprintf(stderr, "Usage: %s <port> [ip]\n", argv[0]);
		exit(-1);
	}

	
	/* 1 - 创建一个 socket 用于 udp 通信 */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket() failed");
		exit(errno);
	}

	/* 2 - 初始化服务器端的地址和端口信息 */
	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(atoi(argv[1]));
	if (argv[2]) {
		/* 监听指定的IP地址 */
		if (inet_pton(AF_INET, argv[2], &(svaddr.sin_addr)) != 1) {
			fprintf(stderr, "inet_pton() convert %s failed: %s\n",
				argv[2], strerror(errno));
			exit(errno);
		}
	} else {
		/* 监听所有IP地址 */
		svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	/* 3 - 绑定监听的地址和端口信息 */
	if (bind(sockfd, (struct sockaddr *) &svaddr, sizeof(svaddr)) == -1) {
		perror("bind() failed");
		exit(errno);
	}



	while (1) {
		/* 4 - 循环接收数据 */
		addr_len = sizeof(cliaddr);

		nbytes = recvfrom(sockfd, buf, BUFSIZE - 1, 0,
			(struct sockaddr *) &cliaddr, &addr_len);
		if (nbytes < 0) {
			perror("recvfrom() failed");
			exit(errno);
		}

		buf[nbytes] = '\0';
		/* 可以使用
		 * inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port)
		 * 但是不推荐 */
		if (inet_ntop(AF_INET, &cliaddr.sin_addr,
			cli_ip, sizeof(cli_ip)) == NULL)
		{
			fprintf(stderr, "inet_ntop() failed\n");
		} else {
			printf("RECV from: %s (%d)\n",
				cli_ip, ntohs(cliaddr.sin_port));
		}
		printf("RECV %d bytes: %s\n", nbytes, buf);
	}

	close(sockfd);

	return 0;
}




