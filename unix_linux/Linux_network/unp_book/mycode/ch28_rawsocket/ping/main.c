/*
 *
 */


#include "ping.h"


struct proto proto_v4 =
	{proc_v4, send_4, NULL, NULL, NULL, 0, IPPROTO_ICMP};
#ifdef IPV6
struct proto proto_v6 =
	{proc_v6, send_6, init_v6, NULL, NULL, 0, IPPROTO_ICMPV6};
#endif

int datalen = 56;	/* data that goes with ICMP each request */


int
main(int argc, char *argv[])
{
	int c;
	struct addrinfo *ai;
	char *h;



	pid = getpid() & 0xFFFF;	/* ICMP ID field is 16 bits */
	signal(SIGALRM, sig_alrm);

	ai = ();
	h = ();
	printf("PING %s (%s): %d data bytes\n", );

	if (ai->ai_family == AF_INET) {
		pr = &proto_v4;
#ifdef IPV6
	} else if (ai->ai_family == AF_INET6) {
		pr = &proto_v6;
#endif
	} else {
		error();
	}

	pr->sasend = ai->ai_addr;
	pr->sarecv = calloc(1, ai->ai_addrlen);
	pr->salen = ai->ai_addrlen;

	readloop();

	return 0;
}





