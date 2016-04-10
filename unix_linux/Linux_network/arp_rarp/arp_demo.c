/*
 *
 */




#define ETH_ALEN		6	/* Octets in one ethernet addr */
#define ETH_P_802_3		0x0001	/* Dummy type for 802.3 frames */


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


#define ARP_REQUEST	0x0001	/* ARP request */
#define ARP_REPLY	0x0002	/* ARP reply */

/* header for arp */
/* /usr/include/net/if_arp.h */
struct arp_header {
	unsigned short arp_hrd;			/* Format of hardware address */
	unsigned short arp_pro;			/* Format of protocol address */
	unsigned char arp_hln;			/* Length of hardware address */
	unsigned char arp_pln;			/* Length of protocol address */
	unsigned short arp_op;			/* ARP opcode (command) */

	unsigned char arp_sha[ETH_ALEN];	/* Sender hardware address */
	unsigned char arp_sip[4];		/* Sender IP address */
	unsigned char arp_tha[ETH_ALEN];	/* Target hardware address */
	unsigned char arp_tip[4];		/* Target IP address */
};


/* struct for arp package */
struct arp_packet
{
	struct ether_header eth;
	struct arp_header arp;
};






send_arp()
{
	/* fill ethernet header */


	/* fill arp header */
}

