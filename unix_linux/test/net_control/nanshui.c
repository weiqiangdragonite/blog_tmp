
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


#define MAXBYTES2CAPTURE	1600

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






/* function prototypes */
static void print_dev_info(pcap_if_t *device);
static void process_packet(u_char *arg, const struct pcap_pkthdr *header, const u_char *packet);




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
	char filter_exp[] = "ip";


	/* find all the devices name that can capture */
	if (pcap_findalldevs(&all_devs, err_buf) == -1) {
		fprintf(stderr, "Error in pcap_findalldevs(): %s\n", err_buf);
		exit(EXIT_FAILURE);
	}

	/* loop all the devices and print their name */
	printf("\nDevices in this computer:\n");
	for (dev = all_devs; dev != NULL; dev = dev->next)
		print_dev_info(dev);

        //printf("Please enter the device name which you want to open:\n>> ");
        //fgets(dev_name, sizeof(dev_name), stdin);
        /* remove the '\n' in the buf */
        //dev_name[strlen(dev_name) - 1] = '\0';
        snprintf(dev_name, sizeof(dev_name), "%s", "re1");

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
	unsigned char is_vlan = 0;			/* is vlan packet? 0:false, 1:true*/
	char src_ip[20];
	char dst_ip[20];
	unsigned short ethernet_type;
	const struct sniff_ethernet *ethernet;		/* The ethernet header */
	const struct sniff_ethernet_vlan *ethernet_vlan;/* The ethernet vlan header */
	struct ip *ipv4;
	struct tcphdr *tcp;
	int cap_size;

	printf("\n");

	id = (unsigned long *) arg;
	++(*id);

	if (*id == 1000)
		exit(EXIT_SUCCESS);

	printf("Packet length: %d\n", header->len);
	printf("Number of bytes: %d\n", header->caplen);


	/*  */
	//printf("-- Ethernet\n");
	ethernet = (struct sniff_ethernet *) packet;

	/* for vlan tag(32 bytes) */
	if (ntohs(ethernet->ether_type) == 0x8100) {
		is_vlan = 1;
		ethernet_vlan = (struct sniff_ethernet_vlan *) packet;
	}


	if (is_vlan)
		ipv4 = (struct ip *) (packet + SIZE_ETHERNET + SIZE_VLAN);
	else
		ipv4 = (struct ip *) (packet + SIZE_ETHERNET);


	memset(src_ip, 0, sizeof(src_ip));
	memset(dst_ip, 0, sizeof(dst_ip));

	strcpy(src_ip, inet_ntoa(ipv4->ip_src));
	strcpy(dst_ip, inet_ntoa(ipv4->ip_dst));

	if (strncmp(src_ip, "10.192.152.", 10) == 0) {
		printf("OUT <--  ");
	} else {
		printf("IN  -->  ");
	}
	printf("src = %s, dst = %s    ", src_ip, dst_ip);
	printf("%s\n", is_vlan ? "VLAN" : "    ");



	ethernet_type = is_vlan ? ntohs(ethernet_vlan->ether_type) : ntohs(ethernet->ether_type);
	/* ethernet type 
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
	}*/

	if (ethernet_type != 0x0800)
		return;

	ipv4->ip_len = 	ntohs(ipv4->ip_len);
	printf("Total Length(Header + Data) - 0x%04x (%d)\n", ipv4->ip_len, ipv4->ip_len);
	printf("Protocol - ");
	switch (ipv4->ip_p) {
	case 1:
		printf("ICMP");
		break;
	case 6:
		printf("TCP");
		break;
	case 17:
		printf("UDP");
		break;
	default:
		printf("???");
		break;
	}
	printf("\n");


	
}
