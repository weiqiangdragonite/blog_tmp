/*
 * http://zhoulifa.bokee.com/6065676.html
 *
 * Filename: simple-udpclient.c
 * Purpose: 基本编程步骤说明，演示了UDP编程的客户端编程步骤
 * Wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
 * Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
 * Date time: 2007-01-24 21:22:00
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




int
main(int argc, char *argv[])
{
	int sockfd;
	int nbytes;
	struct sockaddr_in svaddr;
	socklen_t addr_len;
	char buf[] = "hello, world";


	if (argc != 3) {
		fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
		exit(-1);
	}

	
	/* 1 - 创建一个 socket 用于 udp 通信 */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket() failed");
		exit(errno);
	}

	/* 2 - 设置对方服务器的地址和端口信息 */
	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(atoi(argv[2]));
	if (inet_pton(AF_INET, argv[1], &(svaddr.sin_addr)) != 1) {
		fprintf(stderr, "inet_pton() convert %s failed: %s\n",
			argv[1], strerror(errno));
		exit(errno);
	}


	/* 3 - 发送UDP消息 */
	addr_len = sizeof(svaddr);
	nbytes = sendto(sockfd, buf, sizeof(buf), 0,
			(struct sockaddr *) &svaddr, addr_len);

	if (nbytes < 0) {
		perror("sendto() failed");
		exit(errno);
	}

	close(sockfd);

	return 0;
}




