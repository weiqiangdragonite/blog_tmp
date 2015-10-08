/*
 * for both ipv4 and ipv6
 *
 * str ip --> binary network ip
 *
 * pton --> presentation to numeric

common structure:

struct sockaddr {
	uint8_t		sa_len;
	sa_family_t	sa_family;
	char		sa_data[14];
};


IPv4:

typedef uint32_t in_addr_t;

struct in_addr {
	in_addr_t	s_addr;
};

struct sockaddr_in {
	uint8_t		sin_len;
	sa_family_t	sin_family;
	in_port_t	sin_port;
	struct in_addr	sin_addr;
	char		sin_zero[8];
};


IPv6:

struct in6_addr {
	uint8_t		s6_addr[16];
};

struct sockaddr_in6 {
	uint8_t		sin6_len;
	sa_family_t	sin6_family;
	in_port_t	sin6_port;
	uint32_t	sin6_flowinfo;
	struct in6_addr	sin6_addr;
	uint32_t	sin6_scope_id;
};



 *
 * #include <arpa/inet.h>
 *
 * int inet_pton(int family, const char * strptr, void *addrptr);
 *
 * const char *inet_ntop(int family, const void *addrptr, char *strptr, size_t len);
 *
 */


#include <stdio.h>
#include <arpa/inet.h>


int
main(int argc, char *argv[])
{
	char *addr1 = "192.168.1.255";
	char *addr2 = "fe80::a00:27ff:fe92:2599";
	struct sockaddr_in conv_addr4;
	struct sockaddr_in6 conv_addr6;
	int ret;

	char str4[INET_ADDRSTRLEN];
	char str6[INET6_ADDRSTRLEN];


// IPv4:

	ret = inet_pton(AF_INET, addr1, &conv_addr4.sin_addr);
	if (ret == 0)
		fprintf(stderr, "inet_pton() failed: not valid format\n");
	else if (ret == -1)
		fprintf(stderr, "inet_pton() failed\n");
	else if (ret == 1)
		printf("network order int addr: %#x\n", conv_addr4.sin_addr.s_addr);


	if (inet_ntop(AF_INET, &conv_addr4.sin_addr, str4, sizeof(str4)) == NULL) {
		fprintf(stderr, "inet_ntop() failed\n");
	} else {
		printf("ipv4 str ip: %s\n", str4);
	}


// IPv6:

	ret = inet_pton(AF_INET6, addr2, &conv_addr6.sin6_addr);
	if (ret == 0)
		fprintf(stderr, "inet_pton() failed: not valid format\n");
	else if (ret == -1)
		fprintf(stderr, "inet_pton() failed\n");
	//else if (ret == 1)
	//	printf("network order int addr: %#x\n", conv_addr6.sin6_addr.s6_addr);


	if (inet_ntop(AF_INET6, &conv_addr6.sin6_addr, str6, sizeof(str6)) == NULL) {
		fprintf(stderr, "inet_ntop() failed\n");
	} else {
		printf("ipv6 str ip: %s\n", str6);
	}



	return 0;
}


