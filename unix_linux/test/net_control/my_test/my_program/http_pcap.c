/*
 *
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <pcap.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define __FAVOR_BSD		/* Using BSD TCP header */ 
#include <netinet/tcp.h>
//#define __USE_BSD		/* Using BSD IP header */ 
#include <netinet/ip.h>


#include "circular_buffer.h"
#include "config.h"




#define MAXBYTES2CAPTURE	1518



/****for http url****/
GHashTable *http_hash_white_list; ///ys http 白名单 ip+hosturl
GHashTable *http_hash_person_list;///ys http 个人黑名单 ip+hosturl
GHashTable *http_hash_group_list;///ys http 组黑名单 usergroup+hosturl
GHashTable *http_hash_whole_list;///ys http 全局黑名单 hosturl
GHashTable *http_hash_ALL_w_list;///ys htpp all white，选择ALL，其中一个URL分类整个全选，使用ALL ，白名单
GHashTable *http_hash_ALL_b_list;///ys htpp all black ，黑名单

GHashTable *user_hash_list;///ys 用户哈希表，以IP，查用户



extern GThreadPool *http_process_pool;
extern GError *gerror;




static void process_packet(u_char *arg, const struct pcap_pkthdr *header, const u_char *packet);


void
http_pcap(void)
{
	return;
	char err_buf[PCAP_ERRBUF_SIZE];
	char dev_name[16];
	unsigned long number;

	pcap_t *handle;
	struct bpf_program filter;
	bpf_u_int32 mask;
	bpf_u_int32 net;

	memset(err_buf, 0, PCAP_ERRBUF_SIZE);
	memset(dev_name, 0, sizeof(dev_name));
	handle = NULL;

	// and (not host XXX)
	char filter_exp[] = "tcp port 80 and (tcp[13] != 0x19)";
	snprintf(dev_name, sizeof(dev_name), "p2p1");

	/* get the net id and the mask */
	if (pcap_lookupnet(dev_name, &net, &mask, err_buf) == -1) {
		fprintf(stderr, "Can't get netmask for device %s: %s\n",
			dev_name, err_buf);
		net = 0;
		mask = 0;
		//exit(EXIT_FAILURE);
	}

	handle = pcap_open_live(dev_name, MAXBYTES2CAPTURE, 1, 0, err_buf);

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


	number = 0;
	pcap_loop(handle, -1, process_packet, (u_char *) &number);

	/* close device */
	pcap_freecode(&filter);
	pcap_close(handle);
}

static void
process_packet(u_char *arg, const struct pcap_pkthdr *header, const u_char *packet)
{
	int g_push = 1;
	struct packet_info my_packet;
	memset(&my_packet, 0, sizeof(my_packet));
	my_packet.cap_size = header->caplen;///YS 抓到的长度



/* ========================================================================== */
/*
	unsigned long *id;
	int index, i, j;

	id = (unsigned long *) arg;

	//if (*id == 20)
	//	exit(EXIT_SUCCESS);

	printf("\n------------------------------\n");
	printf("id: %ld\n", ++(*id));
	printf("Packet length: %d\n", header->len);
	printf("Number of bytes: %d\n", header->caplen);
	printf("Recieved time: %s\n",
		ctime((const time_t *) &(header->ts.tv_sec)));

	// print data
	for (index = 0; index < header->len; ) {
		for (i = 0; i < 16; ++index, ++i) {
			if (index < header->len)
				printf("%02x ", packet[index]);
			else
				printf("   ");

			if (i == 7)
				printf(" ");
		}
		printf("    ");
		for (i = index - 16, j = 0; (i < index) && (i < header->len); ++i, ++j) {
			if (isprint(packet[i]))
				printf("%c", packet[i]);
			else
				printf(".");

			if (j == 7)
				printf(" ");
		}
		printf("\n");
	}
	printf("\n\n");

*/
/* ========================================================================== */



	pthread_mutex_lock(mutex_cb_http_catch);
	if (cb_isfull(&cb_http_catch)) {
		///YS 如果缓冲区满了
		///YS 线程池里的线程不够用时，应该要新建线程
		printf("http CB is full\n");
		///YS 线程进入睡眠，并且释放锁，当有notFull_http_a信号出现时，才从睡眠状态醒过来
		pthread_cond_wait(not_full_http_catch, mutex_cb_http_catch);
	}

	// raw_packet 源始数据包的包头
	memcpy(my_packet.raw_packet, packet, header->caplen);

	///YS 写入缓冲区
	cb_write(&cb_http_catch, &my_packet);
	///YS 通知线程池，有数据来了
	g_thread_pool_push(http_process_pool, &g_push, &gerror);

	/* send data to socket */

	pthread_mutex_unlock(mutex_cb_http_catch);
	pthread_cond_signal(not_empty_http_catch);
}


void
http_process(void)
{
	struct packet_info packet_tmp;
	struct ip *ip_header;

	char *start_pos = NULL;
	char *end_pos = NULL;
	int host_len;
	char host_url[256];
	char str_cmp[512];

	memset(&packet_tmp, 0, sizeof(packet_tmp));
	memset(host_url, 0, sizeof(host_url));
	memset(str_cmp, 0, sizeof(str_cmp));


	pthread_mutex_lock(mutex_cb_http_catch);
	///YS 如果是空的话，就等待，直到信号量notEmpty_http_a的产生
	if (cb_isempty(&cb_http_catch)) {
		printf("http CB is empty\n");
		pthread_cond_wait(not_empty_http_catch, mutex_cb_http_catch);
	}


	///YS 从缓冲区里读出一个数据包，写入到packet temp里
	cb_read(&cb_http_catch, &packet_tmp);
	pthread_mutex_unlock(mutex_cb_http_catch);
	pthread_cond_signal(not_full_http_catch);


	///YS ip头信息获取
	ip_header = (struct ip *)(packet_tmp.raw_packet + sizeof(struct ether_header));
	///YS 获取IP_SRC
	strcpy(packet_tmp.src_ip, inet_ntoa(ip_header->ip_src));
	///YS 获取IP_DST
	strcpy(packet_tmp.dst_ip,inet_ntoa(ip_header->ip_dst));


	/* 查到在白名单（个人HTTP ALL，这个人可以无限制地使用80端口）
	此处只对 http_hash_ALL_w_list 生效 */


	///YS 数据包的有效负载 ip_len为IP首部+数据的长度
	// 因为 ip_len不包含ethernet层的14字节，IP+TCP首部的长度为40，减去40后，就是TCP
	// 的数据部分了
	packet_tmp.packet_size = ntohs(ip_header->ip_len) - 40;


	///YS 查找是否有HOST这个字眼
	// memmem - locate a substring
	// 是在整个源数据包查找，保证Host不会在首位出现
	// 问题：同一个数据包有没有可能出现超过两个Host的情况
	start_pos = (char *) memmem(packet_tmp.raw_packet, packet_tmp.cap_size, "Host: ", 6);

	///YS 有的话就取出这个字眼后的内容，直接\R结束，内容通常为：www.baidu.com
	// www.baidu.com\r\n
	if (start_pos) {
		// strchr, strrchr, strchrnul - locate character in string
		end_pos = strchr(start_pos, '\r');

		if (end_pos) {
			// 减去6个字符的Host: 
			host_len = end_pos - start_pos - 6;

			// URL并没有一个最大的长度，要保证不要溢出(最大为255)
			// 不过这里貌似拿到的URL值是域名那部分，后面的参数好像没有拿
			// 所以255字节足够用了
			if (host_len > sizeof(host_url))
				host_len = sizeof(host_url) - 1;

			// +1保证有一位填\0
			snprintf(host_url, host_len + 1, "%s", start_pos + 6);
		}
	}

	if (!host_url[0])
		return;

	//printf("URL -->  %s\n\n", host_url);

	///YS 阻断的标志是HTTP，区分开HTTP，还是TCP
	packet_tmp.block_flag = http_flag;

	strcpy(str_cmp, host_url);
	strcat(str_cmp, packet_tmp.src_ip);

	printf("URL+IP -->  %s\n\n", str_cmp);


	/* 白名单（个人） http_hash_white_list
	这两个白名单有点混淆 */


}













