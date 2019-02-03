/*
 *
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

int main(void)
{
	char *ip_str = "128.128.166.7";
	unsigned char *c = NULL;
	char ip[INET_ADDRSTRLEN];
	struct sockaddr_in addr4;

	memset(ip, 0, sizeof(ip));
	memset(&addr4, 0, sizeof(struct sockaddr_in));

	if (inet_pton(AF_INET, ip_str, &addr4.sin_addr) == 1) {
		c = (unsigned char *) &addr4.sin_addr;
		printf("addr: %x\n", addr4.sin_addr.s_addr);
		printf("Network: %x.%x.%x.%x\n", *c, *(c+1), *(c+2), *(c+3));
		printf("Presentation: %u.%u.%u.%u\n", *c, *(c+1), *(c+2), *(c+3));
	} else {
		perror("inet_pton() error\n");
	}

	if (inet_ntop(AF_INET, &addr4.sin_addr, ip, sizeof(ip)) != NULL)
		printf("ip: %s\n", ip);
	else
		perror("inet_ntop() error\n");


	unsigned char hex[4] = {0x80, 0x80, 0xa6, 0x0e};
	unsigned char hex_1[4] = {0x0e, 0xa6, 0x80, 0x80};
	c = hex;
	printf("Network: %x.%x.%x.%x\n", *c, *(c+1), *(c+2), *(c+3));
	printf("Presentation: %u.%u.%u.%u\n", *c, *(c+1), *(c+2), *(c+3));
	//c = hex_1;
	//printf("Presentation: %u.%u.%u.%u\n", *c, *(c+1), *(c+2), *(c+3));


	return 0;
}
