/*
 * ping - ipv4
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>


#define ICMP_ECHOREPLY	0	/* Echo Reply */
#define ICMP_ECHO	8	/* Echo Request */
#define BUF_SIZE	1024


/*
 * /usr/include/netinet/ip_icmp.h
 */
struct icmp {
	unsigned char	type;		/* 类型 */
	unsigned char	code;		/* 代码 */
	unsigned short	checksum;	/* 校验和 */
	unsigned short	id;		/* 标识符 */
	unsigned short	sequence;	/* 序号 */
	struct timeval	timestamp;	/* 时间戳 */
};

/*
 * /usr/include/netinet/ip.h
 */
struct ip {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned char	hlen: 4;	/* 版本 */
	unsigned char	version: 4;	/* 首部长度 */
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned char	version: 4;	/* 版本 */
	unsigned char	hlen: 4;	/* 首部长度 */
#endif
	unsigned char	tos;		/* 服务类型 */
	unsigned short	len;		/* 总长度 */
	unsigned short	id;		/* 标识符 */
	unsigned short	offset;		/* 标志和片偏移 */
	unsigned char	ttl;		/* 生存时间 */
	unsigned char	protocol;	/* 协议 */
	unsigned short	checksum;	/* 校验和 */
	struct in_addr	ipsrc;		/* 32位源ip地址 */
	struct in_addr	ipdst;		/* 32位目的ip地址 */
};



unsigned short checksum(unsigned short *addr, int len);
float timediff(struct timeval *begin, struct timeval *end);
void pack(struct icmp *p, int sequence);
int unpack(char *buf, int len, char *addr);



int
main(int argc, char *argv[])
{
	struct hostent *host;
	struct icmp sendicmp;
	struct sockaddr_in from;
	struct sockaddr_in to;
	in_addr_t inaddr;
	char buf[BUF_SIZE];

	int sockfd;
	int nsend = 0;
	socklen_t fromlen = 0;
	int nrecv = 0;
	int n = 0;

	memset(&from, 0, sizeof(from));
	memset(&to, 0, sizeof(to));


	if (argc != 2) {
		fprintf(stderr, "Usage: %s <hostname / ip address>\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}


	/* create raw socket */
	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
		fprintf(stderr, "socket() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* set destination address info */
	to.sin_family = AF_INET;

	if ((inaddr = inet_addr(argv[1])) == INADDR_NONE) {
		/* is hostname */
		if ((host = gethostbyname(argv[1])) == NULL) {
			fprintf(stderr, "gethostbyname() failed: %s\n",
				strerror(errno));
			exit(EXIT_FAILURE);
		}
		printf("host %s\n", host->h_addr_list[0]);
		to.sin_addr = *(struct in_addr *) host->h_addr_list[0];
	} else {
		/* is ip address */
		to.sin_addr.s_addr = inaddr;
	}

	/* output ip address info */
	printf("PING %s (%s) : %d bytes of data.\n",
		argv[1], inet_ntoa(to.sin_addr), (int) sizeof(struct icmp));


	while (1) {
		++nsend;
		memset(buf, 0, sizeof(buf));
		/* pack icmp data */
		memset(&sendicmp, 0, sizeof(struct icmp));
		pack(&sendicmp, nsend);

		/* send request */
		if (sendto(sockfd, &sendicmp, sizeof(struct icmp), 0,
		(struct sockaddr *) &to, sizeof(to)) == -1) {
			fprintf(stderr, "sendto() failed: %s\n",
				strerror(errno));
			continue;
		}

		/* recv reply */
		if ((n = recvfrom(sockfd, buf, BUF_SIZE, 0,
		(struct sockaddr *) &from, &fromlen)) < 0) {
			fprintf(stderr, "recvfrom() failed: %s\n",
				strerror(errno));
			continue;
		}

		++nrecv;
		if (unpack(buf, n, inet_ntoa(from.sin_addr)) == -1)
			fprintf(stderr, "unpack() failed\n");

		if (nrecv == 4)
			break;

		sleep(1);
	}

	/* 输出统计信息 */
	printf("--- %s ping statistics ---\n", argv[1]);
	printf("%d packets transmitted, %d received, %%%d packets loss\n",
		nsend, nrecv, (nsend - nrecv) * 100 / nsend);

	return 0;
}



/*
 * addr - 指向需校验数据缓冲区的指针
 * len - 需校验数据的总长度（字节单位）
 */
unsigned short
checksum(unsigned short *addr, int len)
{
    unsigned int sum = 0;  
    while(len > 1) {
        sum += *addr++;
        len -= 2;
    }

    /* 处理剩下的一个字节 */
    if(len == 1)
        sum += *(unsigned char *) addr;

    /* 将32位的高16位与低16位相加 */
    sum = (sum >> 16) + (sum & 0xffff);

    return (unsigned short) ~sum;
}

/*
 * 返回值单位：ms
 * begin - 开始时间戳
 * end - 结束时间戳
 */
float
timediff(struct timeval *begin, struct timeval *end)
{
	int n;
	/* 先计算相差多少微妙 */
	n = (end->tv_sec - begin->tv_sec) * 1000000 +
		(end->tv_usec - begin->tv_usec);

	/* 转化为毫秒返回 */
	return (float) (n / 1000.0);
}



/*
 * p - 指向需要封装的ICMP报文结构体的指针
 * sequence - 该报文的序号
 */
void
pack(struct icmp *p, int sequence)
{
	p->type = ICMP_ECHO;
	p->code = 0;
	p->checksum = 0;
	p->id = getpid();
	p->sequence = sequence;
	gettimeofday(&p->timestamp, NULL);
	p->checksum = checksum((unsigned short *) p, sizeof(struct icmp));
}

/*
 * buf - 指向接收到的IP报文缓冲区的指针
 * len - 接收到的IP报文长度
 * addr - 发送ICMP报文响应的主机IP地址
 */
int
unpack(char *buf, int len, char *addr)
{
	struct ip *ip;
	struct icmp *icmp;
	int ip_head_len;
	float rtt;
	struct timeval end;

	ip = (struct ip *) buf;

	/* 计算ip首部长度，即ip首部的长度标识乘4（4字节为单位） */
	ip_head_len = ip->hlen << 2;

	/* 指向ICMP报文 */
	icmp = (struct icmp *) (buf + ip_head_len);

	/* icmp len */
	len -= ip_head_len;

	/* ICMP报文首部长度小于8 */
	if (len < 8) {
		fprintf(stderr, "ICMP paclets length is less than 8.\n");
		return -1;
	}


	/* 确保是我们所发的ICMP ECHO回应 */
	if (icmp->type != ICMP_ECHOREPLY && icmp->id != getpid()) {
		fprintf(stderr, "ICMP packers are not send by us!\n");
		return -1;
	}

	/* 计算往返时间 */
	/* 这个end的时间应该是在接收数据后计算，然后还要对数据进行checksum */
	gettimeofday(&end, NULL);
	rtt = timediff(&icmp->timestamp, &end);

	/* output ping info */
	printf("%d bytes from %s : icmp_seq=%u ttl=%d rtt=%.2f ms\n",
		len, addr, icmp->sequence, ip->ttl, rtt);

	return 0;
}


