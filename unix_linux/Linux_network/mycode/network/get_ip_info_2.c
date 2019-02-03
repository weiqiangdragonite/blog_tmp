/*
 * 获取所有可用网卡信息
 * 用 getifaddrs 可以获取所有，但没有mac地址。或者获取dev name后，在通过ioctl获取
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>


int main(void)
{
	int ifc_num;
	int sockfd;
	struct ifconf ifc;
	struct ifreq ifr;
	unsigned char buf[512];
	unsigned char hwaddr[6];

	memset(&ifc, 0, sizeof(ifc));
	memset(&ifr, 0, sizeof(ifr));
	memset(hwaddr, 0, sizeof(hwaddr));

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		fprintf(stderr, "socket() error: %s\n", strerror(errno));
		return -1;
	}

	if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0)
	{
		fprintf(stderr, "ioctl() error: %s\n", strerror(errno));
		close(sockfd);
		return -1;
	}

	ifc_num = ifc.ifc_len / sizeof(struct ifreq);
	printf("interface num = [%d]\n", ifc_num);



	struct sockaddr_in *sin = NULL;
	struct ifaddrs *ifa = NULL;
	struct ifaddrs *iflist = NULL;
	int family;

	if (getifaddrs(&iflist) < 0)
	{
		fprintf(stderr, "getifaddrs() error: %s\n", strerror(errno));
		close(sockfd);
		return -1;
	}

	for (ifa = iflist; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
			continue;

		family = ifa->ifa_addr->sa_family;
		printf("interface name = [%s], %s\n", ifa->ifa_name,
			(family == AF_PACKET) ? "(AF_PACKET)" :
			(family == AF_INET) ? "(AF_INET)" :
			(family == AF_INET6) ? "(AF_INET6)" : "(Others)");


		strcpy(ifr.ifr_name, ifa->ifa_name);
		if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0)
		{
			fprintf(stderr, "[%s][%d] ioctl() error: %s\n", 
				__FILE__, __LINE__, strerror(errno));
			close(sockfd);
			return -1;
		}
		if (ifr.ifr_flags & IFF_LOOPBACK)
			continue;	// 回环地址
		if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) < 0)
		{
			fprintf(stderr, "[%s][%d] ioctl() error: %s\n", 
				__FILE__, __LINE__, strerror(errno));
			close(sockfd);
			return -1;
		}

		memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, sizeof(hwaddr));
		printf("\thwaddr: %02x:%02x:%02x:%02x:%02x:%02x\n",
			hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
	}

	freeifaddrs(iflist);


	return 0;
}

