/*
 * gcc -Wall net_data.c -lpcap -o net_data
 *
 * const int *p; --> the variable's value which point to cannot change
 * int const *p; --> cannot point to other variable, but the variable's value can change
 * const int const *p;
 *
 * PS: the network byte order is Big-Endian;
 *     x86 and ARM platform is Little-Endian;
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pcap.h>



/* Ethernet addresses are 6 bytes */
#define MAC_ADDR_LEN	6

/* ethernet headers are always exactly 14 bytes */
#define SIZE_ETHERNET	14

#define IP_ADDR_LEN	4



/* Ethernet header */
struct sniff_ethernet {
	u_char ether_dhost[MAC_ADDR_LEN];	/* Destination host address */
	u_char ether_shost[MAC_ADDR_LEN];	/* Source host address */
	u_short ether_type;			/* IP? ARP? RARP? etc */
};

#define ARP_REQUEST	1
#define ARP_REPLY	2
/* ARP header */
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

/* IP header */
struct sniff_ip {
        u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
        u_char  ip_tos;                 /* type of service */
        u_short ip_len;                 /* total length */
        u_short ip_id;                  /* identification */
        u_short ip_off;                 /* fragment offset field */
        #define IP_RF 0x8000            /* reserved fragment flag */
        #define IP_DF 0x4000            /* dont fragment flag */
        #define IP_MF 0x2000            /* more fragments flag */
        #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
        u_char  ip_ttl;                 /* time to live */
        u_char  ip_p;                   /* protocol */
        u_short ip_sum;                 /* checksum */
        struct  in_addr ip_src,ip_dst;  /* source and dest address */
};
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4)

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
        u_short th_sport;               /* source port */
        u_short th_dport;               /* destination port */
        tcp_seq th_seq;                 /* sequence number */
        tcp_seq th_ack;                 /* acknowledgement number */
        u_char  th_offx2;               /* data offset, rsvd */
#define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
        u_char  th_flags;
        #define TH_FIN  0x01
        #define TH_SYN  0x02
        #define TH_RST  0x04
        #define TH_PUSH 0x08
        #define TH_ACK  0x10
        #define TH_URG  0x20
        #define TH_ECE  0x40
        #define TH_CWR  0x80
        #define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
        u_short th_win;                 /* window */
        u_short th_sum;                 /* checksum */
        u_short th_urp;                 /* urgent pointer */
};



const struct sniff_ethernet *ethernet;		/* The ethernet header */
const struct sniff_arp *arp;
const struct sniff_ip *ip; 			/* The IP header */
const struct sniff_tcp *tcp; 			/* The TCP header */
const char *payload; 				/* Packet payload */

u_int size_ip;
u_int size_tcp;





/* Function prototypes */
static void print_dev_info(pcap_if_t *device);

static void got_packet(u_char *args, const struct pcap_pkthdr *header,
			const u_char *packet);
static void handle_arp_packet(const u_char *packet);


int
main(int argc, char *argv[])
{
	pcap_if_t *all_devs;
	pcap_if_t *dev;
	char err_buf[PCAP_ERRBUF_SIZE];
	char dev_name[16];

	pcap_t *handle;			/* Session handle */
	bpf_u_int32 mask;
	bpf_u_int32 net;
	struct pcap_pkthdr header;	/* The header that pcap gives us */
	const u_char *packet;		/* The actual packet */



	/* Find all the devices */
	if (pcap_findalldevs(&all_devs, err_buf) == -1) {
		fprintf(stderr, "Error in pcap_findalldevs(): %s\n", err_buf);
		exit(EXIT_FAILURE);
	}

	/* Loop all the devices */
	printf("\nDevices in this computer:\n");
	for (dev = all_devs; dev != NULL; dev = dev->next)
		print_dev_info(dev);

	printf("Please enter the device name which you want to open:\n>> ");
	fgets(dev_name, sizeof(dev_name), stdin);
	/* Remove the '\n' in the buf */
	dev_name[strlen(dev_name) - 1] = '\0';

	/* Get the net id and the mask */
	if (pcap_lookupnet(dev_name, &net, &mask, err_buf) == -1) {
		fprintf(stderr, "Can't get netmask for device %s: %s\n",
			dev_name, err_buf);
		net = 0;
		mask = 0;
	}

	/* Open the device */
	handle = pcap_open_live(dev_name, 65535, 1, 0, err_buf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n",
			dev_name, err_buf);
		exit(EXIT_FAILURE);
	}
/*
	if (pcap_datalink(handle) != DLT_EN10MB) {
		fprintf(stderr, "Device %s doesn't provide Ethernet headers - "
			"not supported\n", dev_name);
		exit(EXIT_FAILURE);
	}
*/

	printf("\nCapture Device: %s\n", dev_name);

	printf("------------------------------\n\n");


	int number = 0;
	while (1) {

		/* Grab a packet */
		packet = pcap_next(handle, &header);

		if (packet == NULL)
			continue;

		printf("Packet length: %d\n", header.len);
		printf("Number of bytes: %d\n", header.caplen);
		printf("Recieved time: %s\n",
			ctime((const time_t *) &(header.ts.tv_sec)));

		++number;
		if (number >= 10)
			break;
	}


	/* */
	number = 0;
	pcap_loop(handle, -1, got_packet, (u_char *) &number); 


	/* Close device */
	pcap_close(handle);

	return 0;
}


/*
 *
 */
static void
print_dev_info(pcap_if_t *device)
{
	pcap_addr_t *dev_addr;
	//char ntop4_buf[INET_ADDRSTRLEN];
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
got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	int *id;
	int index;

	id = (int *) args;

	printf("\n------------------------------\n");
	printf("id: %d\n", ++(*id));
	printf("Packet length: %d\n", header->len);
	printf("Number of bytes: %d\n", header->caplen);
	printf("Recieved time: %s\n",
		ctime((const time_t *) &(header->ts.tv_sec)));

	for (index = 0; index < header->len; ++index) {
		printf("%02x ", packet[index]);
		if ((index + 1) % 16 == 0)
			printf("\n");
	}
	printf("\n\n");

	ethernet = (struct sniff_ethernet *) packet;
	printf("Destination - %02x:%02x:%02x:%02x:%02x:%02x\n",
		ethernet->ether_dhost[0], ethernet->ether_dhost[1],
		ethernet->ether_dhost[2], ethernet->ether_dhost[3],
		ethernet->ether_dhost[4], ethernet->ether_dhost[5]);
	printf("Source - %02x:%02x:%02x:%02x:%02x:%02x\n",
		ethernet->ether_shost[0], ethernet->ether_shost[1],
		ethernet->ether_shost[2], ethernet->ether_shost[3],
		ethernet->ether_shost[4], ethernet->ether_shost[5]);

	printf("Type -  %04x\n", ntohs(ethernet->ether_type));

	/* Handle the ARP */
	if (ntohs(ethernet->ether_type) == 0x0806)
		handle_arp_packet(packet);

	/* Handle the TCP */

	printf("\n");
}


/*
 *
 */
static void
handle_arp_packet(const u_char *packet)
{
	arp = (struct sniff_arp *) (packet + SIZE_ETHERNET);

	printf("\nAddress Resolution Protocol (%s)\n",
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








