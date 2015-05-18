/*
 * gcc -Wall simple_sniffer.c -o simple_sniffer -lpcap
 *
 * PLEASE RUN IN ROOT
 *
 * 打开网卡混杂模式
 * ifconfig eth0 promisc
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define __FAVOR_BSD		/* Using BSD TCP header */ 
#include <netinet/tcp.h>
#include <netinet/udp.h>
//#define __USE_BSD		/* Using BSD IP header */ 
#include <netinet/ip.h>

#include <pcap.h>



/*

典型的以太网帧长度是1518字节，但其他的某些协议的数据包会更长一点，但任何一个协议的一个
数据包长度都必然小于65535个字节。

以太网(Ethernet)数据帧的长度必须在46-1500字节之间,这是由以太网的物理特性决定的.    
这个1500字节被称为链路层的MTU(最大传输单元). 
但这并不是指链路层的长度被限制在1500字节,其实这这个MTU指的是链路层的数据区.    
并不包括链路层的首部和尾部的18个字节.

MTU=1500是网络的，网络层的PACKET封装在二层的桢里，
加上source 6B, destination 6B, LENGTH（TYPE）2B, FCS 4B  字段正好1518B

标识UDP长度的字段是16位数字，也就是最长不超过2^16字节，包括UDP首部8字节，楼主可以自己计算下。
数据超过UDP最大长度就分组，具体怎么分就是更高层协议去规定了，也就是应用层的协议。


以太网头长 14 + 4
VLAN长 4
IP头长 20
TCP头长 20


A value of 65535 should be enough to hold any packet from any network.
*/
#define MAXBYTES2CAPTURE	65535


/*

see the libpcapHakin9LuisMartinGarcia.pdf

Data Link Type

Although Ethernet seems to be present everywhere, there are a lot of different
technologies and standards that operate at the data link layer. In order to be
able to decode packets captured from a network interface we must know the
underlying data link type so we are able to interupt the headers used in that
layer.

Data encapsulation in Ethernet networks using the TCP/IP protocol
+----------+-----------+------------+---------+----------+
| ETHERNET | IP HEADER | TCP HEADER | PAYLOAD | ETHERNET |
|  HEADER  |           |            |         | CHECKSUM |
+----------+-----------+------------+---------+----------+


A physical Ethernet packet will look like this:

+-------------------------+--------------------+-------------+-----------+----------------------------+
| Destination MAC address | Source MAC address | Type/Length | User Data | Frame Check Sequence (FCS) |
|           6             |         6          |      2      | 46 - 1500 |           4                |
+-------------------------+--------------------+-------------+-----------+----------------------------+



*/

/*

Network layer protocols and ethertype values

IPv4    0x0800
IPv6    0x86DD
ARP     0x0806
RARP    0x8035



切记：网络数据都是大端模式

比如：0x12345678

小端：
0x12
0x34
0x56
0x78

大端：
0x78
0x56
0x34
0x12

比如以太网的数据包
24 0a 64 2f 6e 3d 00 1a 4b 8a 6a 00 08 00
前6位为目的MAC，接着6位是源MAC，最后两位是上层（即网络层）的协议
这里08 00，即为0x0800，是大端模式
可以用ntohs()转化为小端模式


*/


/* Ethernet addresses are 6 bytes */
#define MAC_ADDR_LEN	6
/* Ethernet headers are always exactly 14 bytes */
#define SIZE_ETHERNET	14
/* VLAN Tag szie */
#define SIZE_VLAN	4
/**/
#define IPV4_HEADER_SIZE	20

#define IP_ADDR_LEN	4

/* Ethernet header */
struct sniff_ethernet {
	u_char ether_dhost[MAC_ADDR_LEN];	/* Destination host address */
	u_char ether_shost[MAC_ADDR_LEN];	/* Source host address */
	u_short ether_type;			/* IP? ARP? RARP? etc */
};

/* Ethernet header with VLAN Tag */
struct sniff_ethernet_vlan {
	u_char ether_dhost[MAC_ADDR_LEN];	/* Destination host address */
	u_char ether_shost[MAC_ADDR_LEN];	/* Source host address */
	u_short vlan_tpid;			/* VLAN Tag data */
	u_short vlan_info;			/* VLAN info */
	u_short ether_type;			/* IP? ARP? RARP? etc */
};



/* ARP Header, (assuming Ethernet+IPv4) */
#define ARP_REQUEST	1
#define ARP_REPLY	2
struct sniff_arp {
	u_short har_type;			/* Hardware type */
	u_short pro_type;			/* Protocol type */
	u_char har_size;			/* Hardware size */
	u_char pro_size;			/* Protocol size */
	u_short opcode;
	u_char send_mac_addr[MAC_ADDR_LEN];
	u_char send_ip_addr[IP_ADDR_LEN];
	u_char recv_mac_addr[MAC_ADDR_LEN];
	u_char recv_ip_addr[IP_ADDR_LEN];
};

/* TCP/IP Header use system defined */


const struct sniff_ethernet *ethernet;		/* The ethernet header */
const struct sniff_ethernet_vlan *ethernet_vlan;/* The ethernet vlan header */
unsigned short ethernet_type;
char type_name[32];
unsigned char is_vlan = 0;			/* is vlan packet? 0:false, 1:true*/



/*
const TYPE *var;           --> var can change, but *var cannot
TYPE const *var;           --> var cannot change, but *var can
const TYPE const *var;
*/
struct sniff_arp *arp;
struct ip *ipv4;
struct tcphdr *tcp;

int cap_size;





/* function prototypes */
static void print_dev_info(pcap_if_t *device);
static void process_packet(u_char *arg, const struct pcap_pkthdr *header, const u_char *packet);
static void handle_arp_packet(const u_char *packet);
static void handle_ipv4_packet(const u_char *packet);
static char *get_protocol_name(unsigned char protocol);
static void handle_tcp_packet(const u_char *packet);



int
main(int argc, char *argv[])
{
	pcap_if_t *all_devs;
	pcap_if_t *dev;
	pcap_t *handle;			/* session handle */
	bpf_u_int32 mask;
	bpf_u_int32 net;
	struct bpf_program filter;	/* Place to store the BPF filter program */
	//struct pcap_pkthdr header;	/* The header that pcap gives us */
	//const u_char *packet;		/* The actual packet(raw data) */

	char err_buf[PCAP_ERRBUF_SIZE];
	char dev_name[16];
	unsigned long number;


	memset(err_buf, 0, PCAP_ERRBUF_SIZE);
	memset(dev_name, 0, sizeof(dev_name));
	handle = NULL;
	//packet = NULL;

	/* BPF filter expression */
/*
example:

"(tcp[13] == 0x10) or (tcp[13] == 0x18)"
"ip"
"tcp"
"udp"
"arp"

tcp[13] == 0x02 and (dst port 22 or dst port 23)
只接收SYN标志位置位且目标端口是22或23的数据包（tcp首部开始的第13个字节）




tcp[13]是控制位
tcp port 80 and (tcp[13] != 0x19)
(0x19 = 11001 --> ACK=1, PSH=1, RST=0, SYN=0, FIN=1)
此处我觉得应该是希望断开连接的包不抓（且接收的数据要立即传给上层应用协议）


tcp and (tcp[13] != 0x4) and (tcp[13] != 0x19) and (tcp[13] != 0x14)
(0x4 = 00100 --> ACK=0, PSH=0, RST=1, SYN=0, FIN=0
0x14 = 10100 --> ACK=1, PSH=0, RST=1, SYN=0, FIN=0
)
我觉得是希望断开连接的包不抓，RST的包不抓

"tcp and ((tcp[13] & 4) != 4)"


控制位（8字节）
+-----+-----+-----+-----+-----+-----+-----+-----+
| CWR | ECE | URG | ACK | PSH | RST | SYN | FIN |
+-----+-----+-----+-----+-----+-----+-----+-----+

CWR与ECE标志都用于IP首部的ECN字段
URG为1表示包中有需要紧急处理的数据
ACK为1时，表示应答字段有效。TCP规定除了最初建立连接时的SYN包之外，该位必须置1
PSH为1时，表示要将收到的数据立刻传给上层应用协议
RST为1时，表示TCP连接中出现异常，必须强制断开连接
SYN为1时，表示希望建立连接
FIN为1时，表示希望断开连接





src host 192.168.1.177
只接收源ip地址是192.168.1.177的数据包

dst port 80
只接收tcp/udp的目的端口是80的数据包

ehter dst 00:e0:09:c1:0e:82
只接收以太网mac地址是00:e0:09:c1:0e:82的数据包

ip[8] == 5
只接收ip的ttl=5的数据包（ip首部开始的第8个字节）


tcp port 80 and (not host 10.192.152.75) and (tcp[13] != 0x19)
tcp port 80
*/
	char filter_exp[] = "tcp port 80";


	/* find all the devices name that can capture */
	if (pcap_findalldevs(&all_devs, err_buf) == -1) {
		fprintf(stderr, "Error in pcap_findalldevs(): %s\n", err_buf);
		exit(EXIT_FAILURE);
	}

	/* loop all the devices and print their name */
	printf("\nDevices in this computer:\n");
	for (dev = all_devs; dev != NULL; dev = dev->next)
		print_dev_info(dev);

	printf("Please enter the device name which you want to open:\n>> ");
	fgets(dev_name, sizeof(dev_name), stdin);
	/* remove the '\n' in the buf */
	dev_name[strlen(dev_name) - 1] = '\0';



	/* get the net id and the mask */
	if (pcap_lookupnet(dev_name, &net, &mask, err_buf) == -1) {
		fprintf(stderr, "Can't get netmask for device %s: %s\n",
			dev_name, err_buf);
		net = 0;
		mask = 0;
		//exit(EXIT_FAILURE);
	}

	/* open the device in promiscuous mode */
	handle = pcap_open_live(dev_name, MAXBYTES2CAPTURE, 1, 0, err_buf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n",
			dev_name, err_buf);
		exit(EXIT_FAILURE);
	}

	/* get the data link type */
	printf("\nLink layer data type is: %d\n"
		"Please see http://www.tcpdump.org/linktypes.html for detail\n",
		pcap_datalink(handle));

	/* Compiles the filter expression into a BPF filter program */
	if (pcap_compile(handle, &filter, filter_exp, 1, mask) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp,
			pcap_geterr(handle));
		exit(EXIT_FAILURE);
	}

	/* Load the filter program into the packet capture device */
	if (pcap_setfilter(handle, &filter) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp,
			pcap_geterr(handle));
		exit(EXIT_FAILURE);
	}


	printf("\nCapture Device: %s\n\n", dev_name);


	number = 0;
	pcap_loop(handle, -1, process_packet, (u_char *) &number);
	// pcap_loop(handle, num_packets, got_packet, NULL);

	/* close device */
	pcap_freecode(&filter);
	pcap_close(handle);

	return 0;
}


/*
 * From libpcap tests example.
 */
static void
print_dev_info(pcap_if_t *device)
{
	pcap_addr_t *dev_addr;

#ifdef INET6
	char ntop6_buf[INET6_ADDRSTRLEN];
#endif


	printf("%s\n", device->name);
	if (device->description)
		printf("\tDescription: %s\n", device->description);
	printf("\tLoopback: %s\n",
		(device->flags & PCAP_IF_LOOPBACK) ? "yes" : "no");


	dev_addr = device->addresses;
	for (; dev_addr != NULL; dev_addr = dev_addr->next) {
		switch (dev_addr->addr->sa_family) {
		case AF_INET:
			printf("\tAddress Family: AF_INET\n");
			if (dev_addr->addr)
				printf("\t\tAddress: %s\n", inet_ntoa(((struct
				sockaddr_in *)(dev_addr->addr))->sin_addr));
			if (dev_addr->netmask)
				printf("\t\tNetmask: %s\n", inet_ntoa(((struct
				sockaddr_in *)(dev_addr->netmask))->sin_addr));
			if (dev_addr->broadaddr)
				printf("\t\tBroadcast Address: %s\n",
				inet_ntoa(((struct sockaddr_in *)
					(dev_addr->broadaddr))->sin_addr));
			if (dev_addr->dstaddr)
				printf("\t\tDestination Address: %s\n",
				inet_ntoa(((struct sockaddr_in *)
					(dev_addr->dstaddr))->sin_addr));
			break;
#ifdef INET6
		case AF_INET6:
			printf("\tAddress Family: AF_INET6\n");
			if (dev_addr->addr)
				printf("\t\tAddress: %s\n", inet_ntop(AF_INET6,
				((struct sockaddr_in6 *)
				(dev_addr->addr))->sin6_addr.s6_addr,
				ntop6_buf, sizeof(ntop6_buf)));
			if (dev_addr->netmask)
				printf("\t\tNetmask: %s\n", inet_ntop(AF_INET6,
				((struct sockaddr_in6 *)
				(dev_addr->netmask))->sin6_addr.s6_addr,
				ntop6_buf, sizeof(ntop6_buf)));
			if (dev_addr->broadaddr)
				printf("\t\tBroadcast Address: %s\n",
				inet_ntop(AF_INET6, ((struct sockaddr_in6 *)
				(dev_addr->broadaddr))->sin6_addr.s6_addr,
				ntop6_buf, sizeof(ntop6_buf)));
			if (dev_addr->dstaddr)
				printf("\t\tDestination Address: %s\n",
				inet_ntop(AF_INET6, ((struct sockaddr_in6 *)
				(dev_addr->dstaddr))->sin6_addr.s6_addr,
				ntop6_buf, sizeof(ntop6_buf)));
			break;
#endif
		default:
			printf("\tAddress Family: Unknown (%d)\n",
				dev_addr->addr->sa_family);
			break;
		}
	}
	printf("\n");
}


/*
 *
 */
static void
process_packet(u_char *arg, const struct pcap_pkthdr *header, const u_char *packet)
{
	unsigned long *id;
	int index, i, j;

	id = (unsigned long *) arg;


	//if (*id == 50)
	//	exit(EXIT_SUCCESS);

	/*
	header->len 是指该数据包的实际长度
	header->caplen 是指我们抓包抓到的长度，受限于 MAXBYTES2CAPTURE，也就是抓包的长度
	不会超过 MAXBYTES2CAPTURE 所定义的值
	*/

	printf("\n------------------------------\n");
	printf("id: %ld\n", ++(*id));
	printf("Packet length: %d\n", header->len);
	printf("Number of bytes: %d\n", header->caplen);
	printf("Received time: %s\n",
		ctime((const time_t *) &(header->ts.tv_sec)));

	cap_size = header->caplen;


	for (index = 0; index < header->len; ) {
		for (i = 0; i < 16; ++index, ++i) {
			if (index < header->len)
				printf("%02x ", packet[index]);
			else
				printf("   ");

			if (i == 7)
				printf(" ");
		}
		printf("    ");
		for (i = index - 16, j = 0; (i < index) && (i < header->len); ++i, ++j) {
			if (isprint(packet[i]))
				printf("%c", packet[i]);
			else
				printf(".");

			if (j == 7)
				printf(" ");
		}
		printf("\n");
	}
	printf("\n\n");


	/*  */
	//printf("-- Ethernet\n");
	ethernet = (struct sniff_ethernet *) packet;

	/* Because the network byte order is Big-Endian, so we will have to
	convert it to our host's order scheme using ntohs() */

	printf("Destination MAC - %02x:%02x:%02x:%02x:%02x:%02x\n",
		ethernet->ether_dhost[0], ethernet->ether_dhost[1],
		ethernet->ether_dhost[2], ethernet->ether_dhost[3],
		ethernet->ether_dhost[4], ethernet->ether_dhost[5]);
	printf("Source MAC - %02x:%02x:%02x:%02x:%02x:%02x\n",
		ethernet->ether_shost[0], ethernet->ether_shost[1],
		ethernet->ether_shost[2], ethernet->ether_shost[3],
		ethernet->ether_shost[4], ethernet->ether_shost[5]);

	/* for vlan tag(32 bytes) */
	if (ntohs(ethernet->ether_type) == 0x8100) {
		is_vlan = 1;
		ethernet_vlan = (struct sniff_ethernet_vlan *) packet;

		/* ... */
		printf("\n* This is a VLAN packet *\n");
		printf("VLAN Tag - 0x%04x%04x\n",
			ntohs(ethernet_vlan->vlan_tpid),
			ntohs(ethernet_vlan->vlan_info));
		printf("TPID - 0x%04x\n", ntohs(ethernet_vlan->vlan_tpid));
		printf("Priority - %d\n", ntohs(ethernet_vlan->vlan_info) >> 13);
		printf("CFI - %d\n", (ntohs(ethernet_vlan->vlan_info) >> 12) & 1);
		printf("VLAN ID - %d\n", ntohs(ethernet_vlan->vlan_info) & 0x0FFF);
	} else {
		is_vlan = 0;
	}


	ethernet_type = is_vlan ? ntohs(ethernet_vlan->ether_type) : ntohs(ethernet->ether_type);

	/* ethernet type */
	memset(type_name, 0, sizeof(type_name));

	switch (ethernet_type) {
	case 0x0800:
		snprintf(type_name, sizeof(type_name), "IPv4");
		break;
	case 0x86DD:
		snprintf(type_name, sizeof(type_name), "IPv6");
		break;
	case 0x0806:
		snprintf(type_name, sizeof(type_name), "ARP");
		break;
	case 0x8035:
		snprintf(type_name, sizeof(type_name), "RARP");
		break;
	case 0x814C:
		snprintf(type_name, sizeof(type_name), "SNMP");
		break;
	default:
		snprintf(type_name, sizeof(type_name), "???");
		break;
	}

	printf("Type - %s (0x%04x)\n", type_name, ethernet_type);


	/* handle arp packet */
	if (ethernet_type == 0x0806)
		handle_arp_packet(packet);

	/* handle ipv4 packet */
	else if (ethernet_type == 0x0800)
		handle_ipv4_packet(packet);
}


/*
 * ARP 只适用于 IPv4，IPv6 可用 ICMPv6 代替
 */
static void
handle_arp_packet(const u_char *packet)
{
	if (is_vlan)
		arp = (struct sniff_arp *) (packet + SIZE_ETHERNET + SIZE_VLAN);
	else
		arp = (struct sniff_arp *) (packet + SIZE_ETHERNET);

	printf("\n-- Address Resolution Protocol (%s)\n",
		(ntohs(arp->opcode) == 1) ? "request" : "reply");

	printf("Hardware type: %s\n", (ntohs(arp->har_type) == 1) ?
		"Ethernet" : "Unknown");
	printf("Protocol type: %04x\n", ntohs(arp->pro_type));
	printf("Hardware size: %d\n", arp->har_size);
	printf("Protocol size: %d\n", arp->pro_size);
	printf("Opcode: %d\n", ntohs(arp->opcode));

	printf("Sender MAC address - %02x:%02x:%02x:%02x:%02x:%02x\n",
		arp->send_mac_addr[0], arp->send_mac_addr[1],
		arp->send_mac_addr[2], arp->send_mac_addr[3],
		arp->send_mac_addr[4], arp->send_mac_addr[5]);
	printf("Sender IP address - %d.%d.%d.%d\n",
		arp->send_ip_addr[0], arp->send_ip_addr[1],
		arp->send_ip_addr[2], arp->send_ip_addr[3]);

	printf("Target MAC address - %02x:%02x:%02x:%02x:%02x:%02x\n",
		arp->recv_mac_addr[0], arp->recv_mac_addr[1],
		arp->recv_mac_addr[2], arp->recv_mac_addr[3],
		arp->recv_mac_addr[4], arp->recv_mac_addr[5]);
	printf("Target IP address - %d.%d.%d.%d\n",
		arp->recv_ip_addr[0], arp->recv_ip_addr[1],
		arp->recv_ip_addr[2], arp->recv_ip_addr[3]);
}


/*
 *
 */
static void
handle_ipv4_packet(const u_char *packet)
{
	if (is_vlan)
		ipv4 = (struct ip *) (packet + SIZE_ETHERNET + SIZE_VLAN);
	else
		ipv4 = (struct ip *) (packet + SIZE_ETHERNET);

	printf("\n-- Internet Protocol Version 4\n");

	/* convert Big-Endian to Little-Endian */
	ipv4->ip_len = 	ntohs(ipv4->ip_len);
	ipv4->ip_id = ntohs(ipv4->ip_id);
	ipv4->ip_off = ntohs(ipv4->ip_off);
	ipv4->ip_sum = ntohs(ipv4->ip_sum);

	printf("Version - %d\n", ipv4->ip_v);
	printf("Header length - %d bytes\n", (ipv4->ip_hl) * 4);
	printf("Type of Service - 0x%02x\n", ipv4->ip_tos);
	printf("Total Length(Header + Data) - 0x%04x (%d)\n", ipv4->ip_len, ipv4->ip_len);
	printf("Identification(分片标识) - 0x%04x (%d)\n", ipv4->ip_id, ipv4->ip_id);
	printf("Flags - 0x%02x\n", (ipv4->ip_off & ~IP_OFFMASK) >> 13);
	printf("  reserved fragment flag: %d, don't fragment flag: %d, "
		"more fragments flag: %d\n", (ipv4->ip_off & IP_RF) >> 15,
		(ipv4->ip_off & IP_DF) >> 14, (ipv4->ip_off & IP_MF) >> 13);
	printf("Fragment offset - %d\n", (ipv4->ip_off & IP_OFFMASK));
	printf("Time to live - %d\n", ipv4->ip_ttl);
	printf("Protocol - %s (%d)\n", get_protocol_name(ipv4->ip_p), ipv4->ip_p);
	printf("Header checksum - 0x%04x\n", ipv4->ip_sum);
	printf("Source address - %s\n", inet_ntoa(ipv4->ip_src));
	printf("Destination address - %s\n", inet_ntoa(ipv4->ip_dst));




	if (ipv4->ip_p == 6)
		handle_tcp_packet(packet);



	//if (strcmp(inet_ntoa(ipv4->ip_dst), "23.22.103.184"))
	//	exit(EXIT_SUCCESS);

}

static char *
get_protocol_name(unsigned char protocol)
{
	switch (protocol) {
	case 1:
		return "ICMP";
	case 6:
		return "TCP";
	case 17:
		return "UDP";
	default:
		return "???";
	}
}


/*
 *
 */
static void
handle_tcp_packet(const u_char *packet)
{
	if (is_vlan)
		tcp = (struct tcphdr *) (packet + SIZE_ETHERNET + SIZE_VLAN + IPV4_HEADER_SIZE);
	else
		tcp = (struct tcphdr *) (packet + SIZE_ETHERNET + IPV4_HEADER_SIZE);

	printf("\n-- Transmission Control Protocol\n");

	printf("Source port - %d\n", ntohs(tcp->th_sport));
	printf("Destination port - %d\n", ntohs(tcp->th_dport));
	printf("Sequence number - 0x%08x\n", ntohl(tcp->th_seq));
	printf("Acknowledgment - 0x%08x\n", ntohl(tcp->th_ack));
	// 数据偏移，可看作是TCP首部的长度
	printf("Data offset - %d\n", tcp->th_off * 4);
	printf("Control Flags - 0x%02x\n", tcp->th_flags);
	printf("  URG:%d, ACK:%d, PSH:%d, RST:%d, SYN:%d, FIN:%d\n",
		(tcp->th_flags & TH_URG) >> 5, (tcp->th_flags & TH_ACK) >> 4,
		(tcp->th_flags & TH_PUSH) >> 3, (tcp->th_flags & TH_RST) >> 2,
		(tcp->th_flags & TH_SYN) >> 1, tcp->th_flags & TH_FIN);
	printf("Window Size - 0x%04x\n", tcp->th_win);
	printf("Checksum - 0x%04x\n", tcp->th_sum);
	printf("Urgent Pointer - 0x%04x\n", tcp->th_urp);

}






