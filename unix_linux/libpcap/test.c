/*
 * run in root
 * gcc -g -Wall test.c -lpcap -o test
 * ldd test
 *
 *
 * www.tcpdump.org
 * http://blog.csdn.net/htttw/article/details/7521053
http://blog.csdn.net/a19881029/article/details/38091243

http://www.programming-pcap.aldabaknocking.com/

 * http://wiki.wireshark.org/Ethernet


安装 http://blog.csdn.net/qinggebuyao/article/details/7715843
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
