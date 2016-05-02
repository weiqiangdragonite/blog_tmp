/*
 * 简单的抓包程序，将那些发给本机的IPv4报文全打印出来
 * 以root身份运行
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <linux/if.h>
#include <linux/sockios.h>

int
main(int argc, char *argv[])
{
	int sockfd, n;
	char buf[2048];
	struct ethhdr *eth;
	struct iphdr *ip;
	struct ifreq ethreq;

	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP))) == -1) {
		perror("socket() failed");
		exit(1);
	}

	/* 设置网卡混杂模式 
	strncpy(ethreq.ifr_name, "enp0s8", IFNAMSIZ);
	if (ioctl(sockfd, SIOCGIFFLAGS, &ethreq) == -1) {
		perror("ioctl() failed");
		close(sockfd);
		exit(1);
	}

	ethreq.ifr_flags |= IFF_PROMISC;
	if (ioctl(sockfd, SIOCGIFFLAGS, &ethreq) == -1) {
		perror("ioctl() failed");
		close(sockfd);
		exit(1);
	} */

	while (1) {
		printf("==============================\n");
		// 注意：在这之前，我没有调用bind函数，原因是什么呢
		n = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
		printf("%d bytes read\n", n);

		// 接收到的数据帧头6字节是目的MAC地址，接着6字节是源MAC地址
		eth = (struct ethhdr *) buf;
		printf("Dest MAC addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
			eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
			eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
		printf("Source MAC addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
			eth->h_source[0], eth->h_source[1], eth->h_source[2],
			eth->h_source[3], eth->h_source[4], eth->h_source[5]);

		ip = (struct iphdr *) (buf + sizeof(struct ethhdr));
		// 我们只对IPv4且没有选项字段的IPv4报文感兴趣
		if (ip->version == 4 && ip->ihl == 5) {
			struct in_addr addr;
			addr.s_addr = ip->saddr;
			printf("Source host: %s\n", inet_ntoa(addr));
			addr.s_addr = ip->daddr;
			printf("Dest host: %s\n", inet_ntoa(addr));
		}
	}

	return 0;
}




