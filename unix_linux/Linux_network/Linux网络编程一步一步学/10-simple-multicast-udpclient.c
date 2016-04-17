/*
 * http://zhoulifa.bokee.com/6066993.html
 *
 * Filename: simple-multicast-udpclient.c
 * Purpose: 演示组播编程的基本步骤，其实这就是一个基本的UDP客户端程序
 * Wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
 * Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
 * Date time: 2007-01-25 13:10:00
 * Note: 任何人可以任意复制代码并运用这些文档，当然包括你的商业用途
 * 但请遵循GPL
 * Thanks to: Google.com
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
	struct sockaddr_in svaddr, cliaddr;
	char buf[1024];


	if (argc != 5) {
		fprintf(stderr, "Usage: %s <dst_ip> <dst_port> "
			"<src_ip> <src_port>\n", argv[0]);
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
	/* 注意这里设置的对方地址是指组播地址，而不是对方的实际IP地址 */
	if (inet_pton(AF_INET, argv[1], &(svaddr.sin_addr)) != 1) {
		fprintf(stderr, "inet_pton() convert %s failed: %s\n",
			argv[1], strerror(errno));
		exit(errno);
	}

	/* 3 - 设置自己的地址和端口信息 */
	memset(&cliaddr, 0, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(atoi(argv[4]));
	if (inet_pton(AF_INET, argv[3], &(cliaddr.sin_addr)) != 1) {
		fprintf(stderr, "inet_pton() convert %s failed: %s\n",
			argv[1], strerror(errno));
		exit(errno);
	}

	/* 4 - 绑定自己的端口和IP信息到socket上 */
	if (bind(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) == -1) {
		perror("bind() failed");
		exit(errno);
	}


	/* 5 - 循环接受用户输入的消息发送组播消息 */
	while (1) {
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf), stdin) == EOF)
			break;

		/* 发送消息 */
		nbytes = sendto(sockfd, buf, strlen(buf), 0,
				(struct sockaddr *) &svaddr, sizeof(svaddr));
		if (nbytes < 0) {
			perror("sendto() failed");
			exit(errno);
		}
	}

	close(sockfd);

	return 0;
}




