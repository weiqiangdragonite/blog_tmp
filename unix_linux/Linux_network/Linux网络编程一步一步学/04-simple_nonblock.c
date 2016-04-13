/*
 * http://zhoulifa.bokee.com/6063041.html
 *
 * Filename: simple-nonblock.c
 * Purpose: 演示最基本的网络编程，循环读取服务器上发过来的内容，直到读完为止
 * Wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
 * Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
 * Date time: 2007-01-23 20:46:54
 * Note: 任何人可以任意复制代码并运用这些文档，当然包括你的商业用途
 * 但请遵循GPL
 * Hope: 希望越来越多的人贡献自己的力量，为科学技术发展出力
 * 科技站在巨人的肩膀上进步更快！感谢有开源前辈的贡献！
 */


/*
问题：

1、非阻塞是什么？
网络通信有阻塞和非阻塞之分，例如对于接收数据的函数recv：
在阻塞方式下，没有数据到达时，即接收不到数据时，程序会停在recv函数这里等待数据的到来；
而在非阻塞方式下就不会等，如果没有数据可接收就立即返回-1表示接收失败。

2、什么是errno？
errno是Linux系统下保存当前状态的一个公共变量，当前程序运行时进行系统调用如果出错，
则会设置errno为某个值以告诉用户出了什么错误。
可以用printf("%d %s\n", errno, strerror(errno));得到具体信息。

3、什么是EAGAIN？
man recv 
当recv系统调用返回这个值时表示recv读数据时，对方没有发送数据过来。
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

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

	/* 6 - 设置 socket 属性为非阻塞方式 */
	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
		perror("fnctl() failed");
		exit(errno);
	}


	/* 7 - 接收对方发过来的消息，每次最多接收 MAXBUF 个字节，*/
	/* 直到把对方发过来的所有消息接收完毕为止 */
	do {
_retry_:
		memset(buffer, 0, MAXBUF + 1);
		nbytes = recv(sockfd, buffer, MAXBUF, 0);

		if (nbytes > 0)
			printf("RECV %ld bytes: %s\n", nbytes, buffer);

		else if (nbytes < 0) {
			if (errno == EAGAIN) {
				printf("data not arrived\n");
				usleep(1000);
				goto _retry_;
			}

			perror("recv() failed");
		}
	} while (nbytes == MAXBUF);

	/* 8 - 关闭连接 */
	close(sockfd);

	return 0;
}

