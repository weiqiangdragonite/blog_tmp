/**
 * This is a tool used to prevent GFW DNS poisoning and return to the correct ip
 * @author chengkai
 * mail chengkai.me@gmail.com
 *
 */


/*
 * I rewrite it for learning, the original code is here
 * https://github.com/examplecode/gfw_dns_resolver
 *
 * <weiqiangdragonite@gmail.com> started on 2016/4/8
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>



/* 看书 TLPI */
#ifndef NI_MAXHOST
#define NI_MAXHOST 256
#endif

#define DNS_SERVER	"8.8.8.8"
#define MAX_WAIT_TIMES	5
#define BUFSIZE		1024


/*
 * 被GFW禁止的IP黑名单(IPv4)
 */
const char BLACK_LIST[][16] = {
	"74.125.127.102",
	"74.125.155.102",
	"74.125.39.102",
	"74.125.39.113",
	"120.55.239.11"
};


struct dnshdr {
	unsigned short trans_id;		/* Transaction ID */
	union {
		unsigned short flags;
		struct {
# if __BYTE_ORDER == __LITTLE_ENDIAN
			unsigned short rcode:4;
			unsigned short zero:3;
			unsigned short ra:1;

			unsigned short rd:1;
			unsigned short tc:1;
			unsigned short aa:1;
			unsigned short opcode:4;
			unsigned short qr:1;
# elif __BYTE_ORDER == __BIG_ENDIAN
			unsigned short qr:1;
			unsigned short opcode:4;
			unsigned short aa:1;
			unsigned short tc:1;
			unsigned short rd:1;

			unsigned short ra:1;
			unsigned short zero:3;
			unsigned short rcode:4;
#endif
		};
	};
	unsigned short questions;
	unsigned short answers;
	unsigned short authority;
	unsigned short additional;
};




void gfw_resolve(const char *hostname, char *out_ip);
void get_hostname(const char *hostname, char *out_ip);
int is_bad_ip(char *ip);
char *build_request_data(const char *hostname, int *size);



/*
 * Usage: ./gfw_dns_resolver www.twitter.com
 * Output: The real ip is: XXX.XXX.XXX.XXX
 */
int
main(int argc, char *argv[])
{
	char real_ip[NI_MAXHOST];

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <domain> ...\n", argv[0]);
		return -1;
	}

	/* 对每个命令行参数进行真正ip的寻找，并输出 */
	while (--argc > 0) {
		gfw_resolve(*++argv, real_ip);
		printf("The %s real ip is: %s\n", *argv, real_ip);
	}

	return 0;
}


/*
 * 
 */
void
gfw_resolve(const char *hostname, char *out_ip)
{
	int sockfd;
	struct sockaddr_in dest;
	socklen_t addr_len;
	char *dns_data = NULL;
	int data_size;
	int i, n;
	char recv_buf[BUFSIZE];

	/* 根据hostname找出ip */
	//get_hostname(hostname, out_ip);

	//printf("out_ip = %s\n", out_ip);


	/* 检测IP是否在黑名单上，如果不是则返回拿到的真实IP */
	//if (is_bad_ip(out_ip) == 0)
		//return;

	/* 如果IP是在黑名单上，则用DNS查找真实的IP */

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("socket() failed");
		exit(errno);
	}


	/* 设置服务器端(对方)地址和端口信息 */
	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(53);
	//dest.sin_addr.s_addr = inet_addr(DNS_SERVER);

	if (inet_pton(AF_INET, DNS_SERVER, &(dest.sin_addr)) != 1) {
		fprintf(stderr, "inet_pton() convert %s failed: %s\n",
			DNS_SERVER, strerror(errno));
		exit(errno);
	}
	addr_len = sizeof(struct sockaddr_in);


	/* 构造DNS数据包 */
	dns_data = build_request_data(hostname, &data_size);

	char *pbuf = dns_data;
	for (i = 0; i < data_size; ++i)
		printf("%02X ", (unsigned char) pbuf[i]);
	printf("\n");


	/* 发送 */
	n = sendto(sockfd, dns_data, data_size, 0,
		(struct sockaddr *) &dest, addr_len);
	if (n == -1) {
		perror("sendto() failed");
		exit(errno);
	}



	for (i = 0; i < MAX_WAIT_TIMES; ++i) {
		memset(recv_buf, 0, BUFSIZE);

		n = recvfrom(sockfd, recv_buf, BUFSIZE, 0,
			(struct sockaddr *) &dest, &addr_len);

		printf("recv %d bytes:\n", n);
		int j = 0;
		for (j = 0; j < n; ++j)
			printf("%02X ", (unsigned char) recv_buf[j]);
		printf("\n");
	}

}




/*
 * 根据hostname找出ip
 */
void
get_hostname(const char *hostname, char *out_ip)
{
	struct sockaddr_in sa;
	struct addrinfo *res;
	int n;


	/** 这个我认为没必要 */
	sa.sin_family = AF_INET;


	/* network address and service translation */
	/* 处理名字到地址以及服务到端口这两种转换 */
	if ((n = getaddrinfo(hostname, NULL, NULL, &res)) != 0) {
		fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(n));
		exit(-1);
	}


	/* 将得到的sockaddr复制到sockaddr_in结构中 */
	memcpy(&sa, res->ai_addr, sizeof(sa));


	/* address-to-name translation */
	/* 以一个套接口地址为参数，返回一个描述主机的字符串和
	 * 一个描述服务的字符串 */
	n = getnameinfo((struct sockaddr *) &sa, sizeof(sa),
		out_ip, NI_MAXHOST, NULL, 0, 0);
	if (n != 0) {
		fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(n));
		exit(-1);
	}

	freeaddrinfo(res);
}



/*
 * 如果得到的IP是在黑名单上的，则返回真 
 */
int
is_bad_ip(char *ip)
{
	int i, n;

	n = sizeof(BLACK_LIST) / sizeof(BLACK_LIST[0]);
	/* 把得到的IP和黑名单上的IP进行对比 */
	for (i = 0; i < n; ++i) {
		/* IP在黑名单上，清除 */
		if (strcmp(BLACK_LIST[i], ip) == 0) {
			printf("got bad ip: %s\n", ip);
			memset(ip, 0, NI_MAXHOST);
			return 1;
		}
	}

	return 0;
}


/*
 * 构造DNS查询数据包
 */
char *
build_request_data(const char *hostname, int *size)
{
	struct dnshdr dns_header;
	/* size = header + (1 + hostname + 1) + type + class */
	*size = sizeof(dns_header) + (1 + strlen(hostname) + 1) + 2 + 2;
	char *data = (char *) calloc(1, *size);
	char *pbuf = data;
	char *pstr = NULL;
	unsigned char len = 0;
	unsigned short query_type;
	unsigned short query_class;


	/* 构造DNS头部，注意大小端 */
	memset(&dns_header, 0, sizeof(dns_header));
	dns_header.trans_id = htons(rand());
	dns_header.qr = 0x00;	/* QR: 查询/响应,1为响应,0为查询 */
	dns_header.tc = 0x00;
	dns_header.rd = 0x01;
	dns_header.flags = htons(dns_header.flags);
	dns_header.questions = htons(0x01);

	/* 复制header到data */
	memcpy(pbuf, &dns_header, sizeof(dns_header));
	pbuf += sizeof(dns_header);


	/* 接着是构造 查询名 + 查询类型 + 查询类 */
	/* 查询名的格式为: 长度(1字节)+N字节内容(N由前面的长度定义)+...+长度0 */
	/* 即比如www.github.com，变为 3www6github3com0 */
	/* 提取hostname中的'.' */
	pstr = strtok((char *) hostname, ".");
	while (pstr != NULL) {
		len = (unsigned char) strlen(pstr);
		pbuf[0] = len;
		pbuf += 1;

		memcpy(pbuf, pstr, len);
		pbuf += len;

		pstr = strtok(NULL, ".");
	}
	pbuf += 1;	/* end */

	/* 查询类型 和 查询类 */
	query_type = htons(0x01);
	query_class = htons(0x01);

	memcpy(pbuf, &query_type, sizeof(query_type));
	pbuf += sizeof(query_type);

	memcpy(pbuf, &query_class, sizeof(query_class));
	pbuf += sizeof(query_class);

	/* debug use:
	pbuf = data;
	int i = 0;
	for (i = 0; i < *size; ++i)
		printf("%02X\n", pbuf[i]);
	*/

	return data;
}



