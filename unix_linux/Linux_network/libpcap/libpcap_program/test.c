/*
 * run in root
 * gcc -g -Wall test.c -lpcap -o test
 * ldd test
 *
 *
 * www.tcpdump.org
 * http://blog.csdn.net/htttw/article/details/7521053
 * http://www.cnblogs.com/Seiyagoo/archive/2012/04/28/2475618.html
 */


#include <stdio.h>
#include <pcap.h>


int
main(void)
{
	printf("sizeof int = %d\n", sizeof int);
	char error_buf[PCAP_ERRBUF_SIZE];
	char *device;

	/* Look up the default device */
	device = pcap_lookupdev(error_buf);

	if (device)
		printf("device: %s\n", device);
	else
		printf("error: %s\n", error_buf);

	return 0;
}
