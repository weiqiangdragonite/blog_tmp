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
	unsigned short questions;	/* 问题数 */
	unsigned short answers;		/* 回答RR数 */
	unsigned short authority;	/* 权威RR数 */
	unsigned short additional;	/* 附加RR数 */
};




void gfw_resolve(const char *hostname, char *out_ip);
void get_hostname(const char *hostname, char *out_ip);
int is_bad_ip(char *ip);
char *build_request_data(char *hostname, int *size);
void decode_dns_response(char *buf, const char *hostname, char *ip);




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
	char *host_buf = NULL;

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
	//dest.sin_addr.s_addr = inet_addr(DNS_SERVER);	// 不推荐inet_addr()
	if (inet_pton(AF_INET, DNS_SERVER, &(dest.sin_addr)) != 1) {
		fprintf(stderr, "inet_pton() convert %s failed: %s\n",
			DNS_SERVER, strerror(errno));
		exit(errno);
	}
	addr_len = sizeof(struct sockaddr_in);


	/* 构造DNS数据包 */
	/* 将hostname复制到host_buf，因为调用函数里的strtok会修改hostname */
	host_buf = (char *) calloc(1, strlen(hostname) + 1);
	memcpy(host_buf, hostname, strlen(hostname));
	dns_data = build_request_data(host_buf, &data_size);


	printf("send dns packet:\n");
	char *pbuf = dns_data;
	for (i = 0; i < data_size; ++i)
		printf("%02X ", (unsigned char) pbuf[i]);
	printf("\n");


	/* 发送DNS数据包 */
	n = sendto(sockfd, dns_data, data_size, 0,
			(struct sockaddr *) &dest, addr_len);
	if (n == -1) {
		perror("sendto() failed");
		exit(errno);
	}


	/* 接收返回的DNS响应包 */
	for (i = 0; i < MAX_WAIT_TIMES; ++i) {
		memset(recv_buf, 0, BUFSIZE);

		n = recvfrom(sockfd, recv_buf, BUFSIZE, 0,
			(struct sockaddr *) &dest, &addr_len);
		if (n == -1) {
			perror("recvfrom() failed");
			exit(errno);
		}

		printf("recv %d bytes:\n", n);
		int j;
		for (j = 0; j < n; ++j)
			printf("%02X ", (unsigned char) recv_buf[j]);
		printf("\n");

		/* 解析返回的响应包 */
		decode_dns_response(recv_buf, hostname, out_ip);
		break;


		/* 再次检测IP是否在黑名单上，如果不在则跳出 */
		//if (is_bad_ip(out_ip) == 0)
			//break;
		/* 如果IP仍在黑名单上，继续等待接收DNS返回的数据，总共尝试
		 * MAX_WAIT_TIMES 次 */
	}

	/* 记得释放 */
	free(dns_data);
	free(host_buf);	/* 这里释放的host_buf会不会有问题，毕竟strtok修改过 */
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
	 * 一个描述服务的字符串
	 * 为什么有些地址拿不到IP地址呢 */
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
build_request_data(char *hostname, int *size)
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
	/* !!! 注意：strtok()会修改hostname的!!!
	 * 因此在调用函数前，我们把hostname复制到host_buf */
	pstr = strtok(hostname, ".");
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



/*
 * 根据DNS的响应报文得到域名对应的IP地址
 */
void
decode_dns_response(char *buf, const char *hostname, char *ip)
{
	struct dnshdr *dns_header = (struct dnshdr *) buf;
	short ans_num = ntohs(dns_header->answers);	/* 回答数量 */
	unsigned char *pbuf;
	int i;
	unsigned short query_type, data_len;

	printf("num = %d\n", ans_num);


	/* 直接跳到 回答区域 */
	/* Answers段有 Name, Type, Class, Time to live, Data length 和Address */
	/* 我们要拿出Address的值从而计算出对应的IP地址 */
	int len = (sizeof(struct dnshdr) + (1 + strlen(hostname) + 1) + 4);
	pbuf = (unsigned char *) (buf + len);


	for (i = 0; i < ans_num; ++i) {
		/* 回答区域一般开头都是 C0 0C
		 * C0：这时不是表示接下来的内容有多长，而是接下来的内容在
		 * 偏移量中，0C:十进制是12的意思，就是偏移12个字节。
		 * 从头开始12位，因为Header结构是固定的12字节，所以偏移0C
		 * 就是到了Question的Name字段 */
		if (pbuf[0] == 0xC0 && pbuf[1] == 0x0C)
			pbuf += 2;
		else
			pbuf += (1 + strlen(hostname) + 1);



		memcpy(&query_type, pbuf, sizeof(query_type));
		query_type = ntohs(query_type);
		//printf("type = 0x%04X\n", query_type);

		pbuf = pbuf + 2 + 2 + 4;
		memcpy(&data_len, pbuf, sizeof(data_len));
		data_len = ntohs(data_len);
		//printf("len = 0x%04X\n", data_len);


		pbuf += 2;
		/* Type 为 A记录 */
		/* 如果有多个A记录，则需要保存对应的IP地址 */
		if (query_type == 0x0001 && data_len == 0x0004) {
			struct sockaddr_in addr;
			memcpy(&addr.sin_addr, pbuf, sizeof(addr.sin_addr));
			if (inet_ntop(AF_INET, &(addr.sin_addr),
				ip, NI_MAXHOST) == NULL)
			{
				perror("inet_pton() failed");
			} else {
				printf("ip = %s\n", ip);
			}
		}

		/* 下一个记录 */
		pbuf += data_len;
	}
}

