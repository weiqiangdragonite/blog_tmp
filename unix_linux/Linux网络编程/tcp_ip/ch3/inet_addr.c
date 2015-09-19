
#include <stdio.h>
#include <arpa/inet.h>

int
main(int argc, char *argv[])
{
	char *addr1 = "192.168.1.101";
	char *addr2 = "1.2.3.256";
	unsigned int conv_addr;

	conv_addr = inet_addr(addr1);
	if (conv_addr == INADDR_NONE)
		fprintf(stderr, "inet_addr() failed\n");
	else
		printf("network order int addr: %#x\n", conv_addr);

	conv_addr = inet_addr(addr2);
	if (conv_addr == INADDR_NONE)
		fprintf(stderr, "inet_addr() failed\n");
	else
		printf("network order int addr: %#x\n", conv_addr);

	return 0;
}





