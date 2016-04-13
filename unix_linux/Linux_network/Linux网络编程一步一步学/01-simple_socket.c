/*
 * http://zhoulifa.bokee.com/6062858.html
 *
 * Filename: simple-socket.c
 * Purpose: 演示最基本的网络编程步骤，这是个客户端程序
 * Wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
 * Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
 * Date time: 2007-01-23 19:41:54
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


#define MAXBUF	1024


int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in svaddr;
	char buffer[MAXBUF];

	if (argc != 3) {
		fprintf(stderr, "Uasge: %s <ip> <port>\n", argv[0]);
		exit(-1);
	}


	/* 1 - 创建一个 socket 用于 tcp 通信 */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket() failed");
		exit(errno);
	}

	/* 2 - 初始化服务器端（对方）的地址和端口信息 */
	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(atoi(argv[2]));
	/** 将字符串IP地址 转化为 网络字节序的IP地址 */
	svaddr.sin_addr.s_addr = inet_addr(argv[1]);
	/** 不推荐使用inet_addr()，使用inet_aton() */
	if (inet_aton(argv[1], (struct in_addr *) &svaddr.sin_addr.s_addr) == 0) {
		fprintf(stderr, "inet_aton() convert %s failed: %s\n",
			argv[1], strerror(errno));
		exit(errno);
	}
	/** inet_aton() 是过时的函数，推荐采用下面的方法 */
	if (inet_pton(AF_INET, argv[1], &(svaddr.sin_addr)) != 1) {
		fprintf(stderr, "inet_pton() convert %s failed: %s\n",
			argv[1], strerror(errno));
		exit(errno);
	}



	/* 3 - 连接服务器 */
	if (connect(sockfd, (struct sockaddr *) &svaddr, sizeof(svaddr)) != 0) {
		perror("connect() failed");
		exit(errno);
	}


	/* 4 - 接收对方发过来的消息，最多接收 MAXBUF 个字节 */
	memset(buffer, 0, MAXBUF);
	recv(sockfd, buffer, sizeof(buffer), 0); /* recv()最后一个参数为0时，等同于read() */
	printf("RECV: %s\n", buffer);


	/* 5 - 关闭连接 */
	close(sockfd);

	return 0;
}


