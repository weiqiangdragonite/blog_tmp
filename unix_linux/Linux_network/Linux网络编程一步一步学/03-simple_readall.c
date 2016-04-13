/*
 * http://zhoulifa.bokee.com/6062989.html
 *
 * Filename: simple-readall.c
 * Purpose: 演示最基本的网络编程，循环读取服务器上发过来的内容，直到读完为止
 * Wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
 * Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
 * Date time: 2007-01-23 20:16:54
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


#define MAXBUF	4


int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in svaddr, cliaddr;
	char buffer[MAXBUF + 1];
	ssize_t nbytes;

	if (argc != 5) {
		fprintf(stderr, "Usage: %s <dest_ip> <dest_port> "
			"<src_ip> <src_port>\n", argv[0]);
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
	if (inet_pton(AF_INET, argv[1], &(svaddr.sin_addr)) != 1) {
		fprintf(stderr, "inet_pton() convert %s failed: %s\n",
			argv[1], strerror(errno));
		exit(errno);
	}

	/* 3 - 初始化自己的地址和端口信息 */
	memset(&cliaddr, 0, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(atoi(argv[4]));
	if (inet_pton(AF_INET, argv[3], &(cliaddr.sin_addr)) != 1) {
		fprintf(stderr, "inet_pton() convert %s failed: %s\n",
			argv[3], strerror(errno));
		exit(errno);
	}

	/* 4 - 把自己的 IP 地址信息和端口与 socket 绑定 */
	if (bind(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) == -1) {
		perror("bind() failed");
		exit(errno);
	}

	/* 5 - 连接服务器 */
	if (connect(sockfd, (struct sockaddr *) &svaddr, sizeof(svaddr)) == -1) {
		perror("connect() failed");
		exit(errno);
	}

	/* 6 - 接收对方发过来的消息，每次最多接收 MAXBUF 个字节，*/
	/* 直到把对方发过来的所有消息接收完毕为止 */
	do {
		memset(buffer, 0, MAXBUF + 1);
		nbytes = recv(sockfd, buffer, MAXBUF, 0);
		printf("RECV %ld bytes: %s\n", nbytes, buffer);
	} while (nbytes == MAXBUF);

	/* 7 - 关闭连接 */
	close(sockfd);

	return 0;
}

