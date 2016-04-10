/**
 * This is a tool used to prevent GFW DNS poisoning and return to the correct ip
 * @author chengkai
 * mail chengkai.me@gmail.com
 *
 */


/*
 * I rewrite it for learning, the original code is here
 * https://github.com/examplecode/gfw_dns_resolver
 *
 * <weiqiangdragonite@gmail.com> started on 2016/4/8
 */


#include <stdio.h>

#include <sys/socket.h>
#include <netdb.h>



/* 看书 TLPI */
#ifndef NI_MAXHOST
#define NI_MAXHOST 256
#endif


/*
 * Usage: ./gfw_dns_resolver www.twitter.com
 * Output: The real ip is: XXX.XXX.XXX.XXX
 */
int
main(int argc, char *argv[])
{
	char real_ip[NI_MAXHOST];

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <domain> ...\n", argv[0]);
		return -1;
	}

	/* 对每个命令行参数进行真正ip的寻找，并输出 */
	while (--argc > 0) {
		gfw_resolve(*++argv, real_ip);
		printf("The %s real ip is: %s\n", *argv, real_ip);
	}

	return 0;
}


void
gfw_resolve(const char *hostname, char *out_ip)
{


	/**/
	get_hostname(hostname, out_ip);


	/**/
	if (is_bad_ip(out_ip))
		return;
}




/*
 *
 */
get_hostname(const char *hostname, char *out_ip)
{
	struct sockaddr_in sa;
	struct addrinfo *res;
	int n;


	/** ??? */
	sa.sin_family = AF_INET;


	/* network address and service translation */
	/* 处理名字到地址以及服务到端口这两种转换 */
	if ((n = getaddrinfo(hostname, NULL, NULL, &res)) != 0) {
		fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(n));
		exit(-1);
	}


	/** ??? */
	memcpy(&sa, res->ai_addr, sizeof(sa));


	/* address-to-name translation */
	/* 以一个套接口地址为参数，返回一个描述主机的字符串和
	 * 一个描述服务的字符串 */
	n = getnameinfo((struct sockaddr *) &sa, sizeof(sa),
		out_ip, NI_MAXHOST, NULL, 0, 0);
	if (n != 0) {
		fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(n));
		exit(-1);
	}

	freeaddrinfo(res);
}



/*
 * 
 */
is_bad_ip()
{

}



