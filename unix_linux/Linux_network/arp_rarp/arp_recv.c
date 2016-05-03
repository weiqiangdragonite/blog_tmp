/*
 * arp receiver
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netpacket/packet.h>




/* /usr/include/linux/if_ether.h */
#define ETH_ALEN		6	/* Octets in one ethernet addr */
#define ETH_FRAME_LEN		1514	/* Max. octets in frame sans FCS */

#define ETH_P_802_3		0x0001	/* Dummy type for 802.3 frames */

#define ETH_P_ARP		0x0806	/* Address Resolution packet    */
#define ETH_P_ALL		0x0003	/* Every packet (be careful!!!) */

/* /usr/include/net/ethernet.h */
#define ETHERTYPE_IP		0x0800	/* IP */
#define ETHERTYPE_ARP		0x0806	/* ARP: Address resolution */
#define ETHERTYPE_REVARP	0x8035	/* RARP: Reverse ARP */


/* header for ethernet */
/* /usr/include/linux/if_ether.h */
/* /usr/include/net/ethernet.h */
struct ether_header {
	unsigned char	ether_dst[ETH_ALEN];	/* destination ethernet address */
	unsigned char	ether_src[ETH_ALEN];	/* source ethernet address */
	unsigned short	ether_type;		/* ethernet pachet type */
};


#define ARPHRD_ETHER	1	/* Ethernet 10/100Mbps.  */
#define ARP_REQUEST	0x0001	/* ARP request */
#define ARP_REPLY	0x0002	/* ARP reply */

/* header for arp */
/* /usr/include/net/if_arp.h */
/* /usr/include/netinet/if_ether.h */
struct arp_header {
	unsigned short arp_hrd;			/* Format of hardware address */
	unsigned short arp_pro;			/* Format of protocol address */
	unsigned char arp_hln;			/* Length of hardware address */
	unsigned char arp_pln;			/* Length of protocol address */
	unsigned short arp_op;			/* ARP opcode (command) */

	unsigned char arp_sha[ETH_ALEN];	/* Sender hardware address */
	unsigned char arp_spa[4];		/* Sender IP address */
	unsigned char arp_tha[ETH_ALEN];	/* Target hardware address */
	unsigned char arp_tpa[4];		/* Target IP address */
};


/* struct for arp package */
struct arp_packet
{
	struct ether_header eth;
	struct arp_header arp;
};




int
main(int argc, char *argv[])
{
	int sfd, n, i;
	struct in_addr dst_addr, src_addr;

	/* netpacket/packet.h */
	struct sockaddr_ll fromaddr;	/* 不能使用struct sockaddr_in结构 */
	struct ifreq ifr;		/* linux/if_ether.h */

	unsigned char src_mac[ETH_ALEN] = { 0 };
	struct arp_packet *packet;
	char buf[ETH_FRAME_LEN] = { 0 };


	if (argc != 2) {
		fprintf(stderr, "Usage: %s <dev_name>\n", argv[0]);
		exit(1);
	}

	/* 只接收发给本机的ARP报文 */
	if ((sfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) == -1) {
		perror("socket() failed");
		exit(1);
	}

	memset(&fromaddr, 0, sizeof(fromaddr));

	/* 获取网卡接口索引 */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, argv[1], IFNAMSIZ);
	if (ioctl(sfd, SIOCGIFINDEX, &ifr) == -1) {
		perror("get dev index error");
		exit(1);
	}
	fromaddr.sll_ifindex = ifr.ifr_ifindex;
	printf("interface index: %d\n", ifr.ifr_ifindex);


	/* 获取网卡接口MAC地址 */
	if (ioctl(sfd, SIOCGIFHWADDR, &ifr) == -1) {
		perror("get dev mac addr error");
		exit(1);
	} else {
		memcpy(src_mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
		printf("MAC: %02X-%02X-%02X-%02X-%02X-%02X\n",
			src_mac[0], src_mac[1], src_mac[2], src_mac[3],
			src_mac[4], src_mac[5]);
	}


	fromaddr.sll_family = PF_PACKET;
	fromaddr.sll_protocol = htons(ETH_P_ARP);
	fromaddr.sll_hatype = ARPHRD_ETHER;
	fromaddr.sll_pkttype = PACKET_HOST;
	fromaddr.sll_halen = ETH_ALEN;

	memcpy(fromaddr.sll_addr, src_mac, ETH_ALEN);

	bind(sfd, (struct sockaddr *) &fromaddr, sizeof(struct sockaddr));

	printf("start recving ...\n");
	while (1) {
		printf("\n");
		memset(buf, 0, sizeof(buf));
		n = recvfrom(sfd, buf, sizeof(buf), 0, NULL, NULL);

		packet = (struct arp_packet *) buf;

		printf("Dest MAC: ");
		for (i = 0; i < ETH_ALEN - 1; ++i)
			printf("%02X-", packet->eth.ether_dst[i]);
		printf("%02X\n", packet->eth.ether_dst[i]);

		printf("Src MAC: ");
		for (i = 0; i < ETH_ALEN - 1; ++i)
			printf("%02X-", packet->eth.ether_src[i]);
		printf("%02X\n", packet->eth.ether_src[i]);

		printf("Frame type: %04X\n", ntohs(packet->eth.ether_type));


		if (ntohs(packet->arp.arp_op) == ARP_REQUEST) {
			printf("Get ARP request\n");
		} else if (ntohs(packet->arp.arp_op) == ARP_REPLY) {
			printf("Get ARP reply\n");
		}
	}



	close(sfd);
	return 0;
}









