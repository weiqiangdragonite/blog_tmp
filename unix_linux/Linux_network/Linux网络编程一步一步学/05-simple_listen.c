/*
 * http://zhoulifa.bokee.com/6064410.html
 *
 * Filename: simple-listen.c
 * Purpose: 演示最基本的网络编程步骤，开启服务端的监听，等待客户端来连接
 * Wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
 * Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
 * Date time: 2007-01-24 12:31:00
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
	int sockfd, clifd;
	int nbytes;
	struct sockaddr_in svaddr, cliaddr;
	socklen_t addr_len;
	char buf[] = "hello, world";

	if (argc != 2 && argc != 3) {
		fprintf(stderr, "Usage: %s <port> [ip]\n", argv[0]);
		exit(-1);
	}

	
	/* 1 - 创建一个 socket 用于 tcp 通信 */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
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

	/* 3 - 绑定监听的地址 */
	if (bind(sockfd, (struct sockaddr *) &svaddr, sizeof(svaddr)) == -1) {
		perror("bind() failed");
		exit(errno);
	}

	/* 4 - 设置监听队列长度 */
	if (listen(sockfd, 5) == -1) {
		perror("listen() failed");
		exit(errno);
	}

	/* 5 - 等待连接 */
	addr_len = sizeof(cliaddr);
	clifd = accept(sockfd, (struct sockaddr *) &cliaddr, &addr_len);
	if (clifd == -1) {
		perror("accept() failed");
		exit(errno);
	}

	/* 6 - 发送数据给客户端 */
	if ((nbytes = write(clifd, buf, sizeof(buf))) != sizeof(buf))
		perror("write() failed");
	printf("write %d bytes\n", nbytes);


	/* 7 - 关闭连接 */
	close(clifd);
	close(sockfd);

	return 0;
}




