/*
 * http://zhoulifa.bokee.com/6065720.html
 *
 * Filename: simple-broadcast-udpclient.c
 * Purpose: 基本编程步骤说明，演示了UDP编程的广播客户端编程步骤
 * Wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
 * Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
 * Date time: 2007-01-24 21:30:00
 * Note: 任何人可以任意复制代码并运用这些文档，当然包括你的商业用途
 * 但请遵循GPL
 * Thanks to: Google.com
 * Hope: 希望越来越多的人贡献自己的力量，为科学技术发展出力
 * 科技站在巨人的肩膀上进步更快！感谢有开源前辈的贡献！
 *
 * ./client 192.168.0.255 7838
 * 就会往192.168.0网络内所有主机发消息。
 */

/*
广播数据包的原理：
专门用于同时向网络中所有工作站进行发送的一个地址叫做广播地址。在使用TCP/IP 协议
的网络中，主机标识段host ID 为全1 的IP 地址为广播地址。如果你的IP为：
192.168.1.39，子网掩码为：255.255.255.0，则广播地址为：192.168.1.255；
如果IP为192.168.1.39，子网掩码为：255.255.255.192，则广播地址为：192.168.1.63。

如果只想在本网络内广播数据，只要向广播地址发送数据包即可，这种数据包可以被路由，
它会经由路由器到达本网段内的所有主机，此种广播也叫直接广播；如果想在整个网络中
广播数据，要向255.255.255.255发送数据包，这种数据包不会被路由，它只能到达本
物理网络中的所有主机，此种广播叫有限广播。


注意事项如下：
1. 接收方一定要知道广播方的口号，然后绑定此端口号才能正确接收。
2. 接收方的Socket不需要设置成广播属性。
3. 绑定的IP不可以使用“127.0.0.1”，可以使用真实IP地址或者INADDR_ANY。否则接收失败。
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
	int optval;


	if (argc != 3) {
		fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
		exit(-1);
	}

	
	/* 1 - 创建一个 socket 用于 udp 通信 */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket() failed");
		exit(errno);
	}


	/* 2 - 设置通讯方式对广播，即本程序发送的一个消息,
	 * 网络上所有主机均可以收到 */
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));



	/* 3 - 设置广播地址和端口信息 */
	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(atoi(argv[2]));
	if (inet_pton(AF_INET, argv[1], &(svaddr.sin_addr)) != 1) {
		fprintf(stderr, "inet_pton() convert %s failed: %s\n",
			argv[1], strerror(errno));
		exit(errno);
	}


	/* 4 - 发送UDP消息 */
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




