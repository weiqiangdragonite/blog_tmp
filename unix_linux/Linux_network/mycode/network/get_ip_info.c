/*
 * 通过 struct ifconf 和 struct ifreq 获取网线插入状态
 * 这个并非是获取网线插入状态，只能获取到有ip地址的网卡名
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>


int main(void)
{
	int i = 0;
	int sockfd;
	struct ifconf ifcon;
	struct ifreq *ifr;
	unsigned char buf[512];

	// init
	ifcon.ifc_len = 512;
	ifcon.ifc_buf = buf;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		fprintf(stderr, "socket() error: %s\n", strerror(errno));
		return -1;
	}

	// 获取所有接口信息
	if (ioctl(sockfd, SIOCGIFCONF, &ifcon) < 0)
	{
		fprintf(stderr, "ioctl() error: %s\n", strerror(errno));
		close(sockfd);
		return -1;
	}

	// 获取一个一个的ip地址
	ifr = (struct ifreq *) buf;
	for (i = ifcon.ifc_len / sizeof(struct ifreq); i > 0; --i)
	{
		printf("name = [%s]\n", ifr->ifr_name);
		printf("local addr = [%s]\n", inet_ntoa(((struct sockaddr_in *) &(ifr->ifr_addr))->sin_addr));
		ifr++;
	}

	return 0;
}

