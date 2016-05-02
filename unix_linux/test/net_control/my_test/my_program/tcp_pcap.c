/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include <glib.h>
#include <pcap.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define __FAVOR_BSD		/* Using BSD TCP header */ 
#include <netinet/tcp.h>
#include <netinet/udp.h>
//#define __USE_BSD		/* Using BSD IP header */ 
#include <netinet/ip.h>
#



#include "circular_buffer.h"
#include "config.h"
#include "packet_info.h"



#define MAXBYTES2CAPTURE	65535


GHashTable *tcp_hash_white_list;
GHashTable *tcp_hash_person_list;
GHashTable *tcp_hash_group_list;
GHashTable *tcp_hash_whole_list;
GHashTable *qq_ip_list;


extern GThreadPool *tcp_process_pool;
extern GError *gerror;
extern GHashTable *tcp_port_list;
GHashTable *tcp_stream_list;

struct user_flow userf[255];
char net_addr[8] = "192.168";
int mallco_num=0;
int http_enable = 1;///YS HTTP访问是否打开


static void process_packet(u_char *arg, const struct pcap_pkthdr *header, const u_char *packet);
int flow_count(struct packet_info *packet_tmp);
int get_pro_and_ip(struct packet_info *packet_tmp);
int store_packet(struct packet_info *packet_tmp2);
void dbug(char *fmt, ...);
int process_out_stream(char *in_file,char *out_file,char *ip);

extern int create_tmp_file( struct stream * s,int in_len,int out_len);



void
tcp_pcap(void)
{
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

	//char filter_exp[] = "tcp and (tcp[13] != 0x4) and (tcp[13] != 0x19) and (tcp[13] != 0x14)";
	char filter_exp[] = "tcp port 80 and (host 192.168.1.211)";
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
	my_packet.cap_size = header->caplen;///YS 抓到的实际长度
	my_packet.cap_time = header->ts;// 抓到的时间



/* ========================================================================== */

	unsigned long *id;
	int index, i, j;

	id = (unsigned long *) arg;

	if (*id == 30)
		exit(EXIT_SUCCESS);

	printf("\n------------------------------\n");
	printf("id: %ld\n", ++(*id));
	printf("Packet length: %d\n", header->len);
	printf("Number of bytes: %d\n", header->caplen);
	printf("Recieved time: %s\n",
		ctime((const time_t *) &(header->ts.tv_sec)));


if (header->caplen < 80) {
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

}

/* ========================================================================== */



	pthread_mutex_lock(mutex_cb_tcp_catch);
	if (cb_isfull(&cb_tcp_catch)) {
		///YS 如果缓冲区满了
		///YS 线程池里的线程不够用时，应该要新建线程
		printf("tcp CB is full\n");
		///YS 线程进入睡眠，并且释放锁，当有notFull_tcp_a信号出现时，才从睡眠状态醒过来
		pthread_cond_wait(not_full_tcp_catch, mutex_cb_tcp_catch);
	}

	// raw_packet 源始数据包的包头
	memcpy(my_packet.raw_packet, packet, header->caplen);

	///YS 写入缓冲区，等待分析拦截
	cb_write(&cb_tcp_catch, &my_packet);
	///YS 通知线程池，有数据来了
	g_thread_pool_push(tcp_process_pool, &g_push, &gerror);

	pthread_mutex_unlock(mutex_cb_tcp_catch);
	pthread_cond_signal(not_empty_tcp_catch);



	/* send data to socket */
	/* 写到审计缓冲区，等待分析审计 */
	pthread_mutex_lock(mutex_cb_tcp_audit);
	if (cb_isfull(&cb_tcp_audit))
		pthread_cond_wait(not_full_tcp_audit, mutex_cb_tcp_audit);
	cb_write(&cb_tcp_audit, &my_packet);
	pthread_cond_signal(not_empty_tcp_audit);
	pthread_mutex_unlock(mutex_cb_tcp_audit);
}



void
tcp_process(void)
{
	struct packet_info packet_tmp;
	struct tcphdr *tcp_header = NULL;
	struct ip *ip_header = NULL;

	char *start_pos = NULL;
	char *end_pos = NULL;
	int host_len;
	char host_url[256];
	char str_cmp[512];
	char port[8]= { '\0' };
	int port_flag=1;
	char tcp_get_tmp[64] = { '\0' };
	time_t timep;

	time(&timep);

	memset(&packet_tmp, 0, sizeof(packet_tmp));
	memset(host_url, 0, sizeof(host_url));
	memset(str_cmp, 0, sizeof(str_cmp));


	pthread_mutex_lock(mutex_cb_tcp_catch);
	///YS 如果是空的话，就等待，直到信号量notEmpty_tcp_a的产生
	if (cb_isempty(&cb_tcp_catch)) {
		printf("tcp CB is empty\n");
		pthread_cond_wait(not_empty_tcp_catch, mutex_cb_tcp_catch);
	}

	///YS 从缓冲区里读出一个数据包，写入到packet temp里
	cb_read(&cb_tcp_catch, &packet_tmp);
	pthread_mutex_unlock(mutex_cb_tcp_catch);
	pthread_cond_signal(not_full_tcp_catch);


	///YS ip头信息获取
	ip_header = (struct ip *)(packet_tmp.raw_packet + sizeof(struct ether_header));
	///YS 获取IP_SRC
	strcpy(packet_tmp.src_ip, inet_ntoa(ip_header->ip_src));
	///YS 获取IP_DST
	strcpy(packet_tmp.dst_ip,inet_ntoa(ip_header->ip_dst));


	tcp_header= (struct tcphdr *) (packet_tmp.raw_packet + sizeof(struct ether_header) + sizeof(struct ip));
	packet_tmp.src_port = ntohs(tcp_header->th_sport);
	snprintf(port, sizeof(port), "%d", packet_tmp.src_port);


	///YS 找一下这个数据包里的端口是不是在我们的端口哈希表里存在，例如，23TELNET这一类的
	if (g_hash_table_lookup(tcp_port_list, port)) {
		///YS 找到端口的话，那就证明这个包是采用固定端口通信的，prot_flag=2
		strcat(str_cmp, packet_tmp.dst_ip);
		strcat(str_cmp, port);///YS 这样的话要查哈希表的KEY，是IP+PROT
		port_flag = 2;

		//printf("IP + PORT --> %s\n", str_cmp);
	} else {
		strcat(str_cmp, packet_tmp.src_ip);
		///YS 否则没有找到固定端口，要查哈希表的KEY是：serverip+useripys
		strcat(str_cmp, packet_tmp.dst_ip);
		strcpy(tcp_get_tmp, packet_tmp.dst_ip);

		//printf("SERVER IP + USER IP --> %s\n", str_cmp);
	}



	/**/
	if (g_hash_table_lookup(tcp_hash_white_list, str_cmp)) {
		//dbug("in while list !!!!!!");    ///YS 白名单IP+IP
		//dbug(str_cmp);
		return;
	}


	/* 个人黑名单 */
	if (g_hash_table_lookup(tcp_hash_person_list, str_cmp)) {
		//dbug("in person list!!!! %s", str_cmp);
		g_hash_table_insert(tcp_hash_person_list, g_strdup(str_cmp), (gpointer) timep);


		pthread_mutex_lock(mutex_cb_http_intercept);
		if (cb_isfull(&cb_http_intercept)) {
			//dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
			printf("http intercept CB is full\n");
			pthread_cond_wait(not_full_http_intercept, mutex_cb_http_intercept);
		}

		cb_write(&cb_http_intercept, &packet_tmp);///YS 在单名单里，把数据包写入拦截缓冲
		//g_thread_pool_push(pool_tcp_rst, &g_push, &gerror);///YS 通知拦截线程池

		pthread_mutex_unlock(mutex_cb_http_intercept);
		pthread_cond_signal(not_empty_http_intercept);

		return;
	}


}


/**************************
获取一个包，并且创建要审计的流
**************************/
void
get_syn_and_build_stream(void)
{
	struct packet_info my_packet;
	memset(&my_packet, 0, sizeof(struct packet_info));


	while (1) {
		pthread_mutex_lock(mutex_cb_tcp_audit);
		if (cb_isempty(&cb_tcp_audit)) {
			//dbug("empty and wait \n\n");
			//printf("audit CB is empty");
			pthread_cond_wait(not_empty_tcp_audit, mutex_cb_tcp_audit);
		}

		//printf("audit CB is not empty\n");
		cb_read(&cb_tcp_audit, &my_packet);///YS 获取一个packet
		pthread_mutex_unlock(mutex_cb_tcp_audit);
		pthread_cond_signal(not_full_tcp_audit);


		//if the syn packet
		// 主要是判断一下是udp还是tcp，然后设置一下相应的信息，并创建流的名字
		get_pro_and_ip(&my_packet);

		flow_count(&my_packet);///YS 计算用户的流量
		//dbug("!!!!!!!!!!   %d\n",my_packet.cap_size);

/*
		if (my_packet.cap_size > 62) {
			//process_im(&my_packet);
		}

		///YS 查一下哈希表，得出该数据包的协议，是http or tftp or ftp or smtp or pop3
		if (!find_pro_from_hash(&my_packet))
			continue;


		///YS 没有找到port时，就再看一下是不是UDP，是的话就处理一下UDP
		if (my_packet.packet_type == Udp) {
			//dbug("udp ~~~~~~~~~ %s\n",my_packet.behavior_type);
			udp_process(&my_packet);
		}
*/

		///YS 如果该包是TCP的话
		if (my_packet.packet_type == Tcp) {

			///YS remove the tcp stream
			// FIN或RST的包
			if ((my_packet.flat & TH_FIN) || (my_packet.flat & TH_RST)) {
				printf("FIN or RST\n");
				//remove_stream(&my_packet);
			}

			///YS build the tcp stream
			// 是SYN或ACK包
			if ((my_packet.flat & TH_SYN) || (my_packet.flat & TH_ACK)) {
				printf("SYN or ACK\n");
				store_packet(&my_packet);
			}
		}

	}

}


/***********************
获取数据包中的端口和IP等信息
参数1，是我们要处理的数据包
***********************/
int
get_pro_and_ip(struct packet_info *packet_tmp)
{
	struct tcphdr *tcp_header;// tcp header
	struct ip * ip_header;//ip header
	struct udphdr *udp;


	ip_header = (struct ip *)(packet_tmp->raw_packet + sizeof(struct ether_header));

	memset(packet_tmp->dst_ip, 0, 32);
	memset(packet_tmp->src_ip, 0, 32);

	strcpy(packet_tmp->dst_ip, inet_ntoa(ip_header->ip_dst));///YS 获取IP
	strcpy(packet_tmp->src_ip, inet_ntoa(ip_header->ip_src));


	///YS 如果是tcp
	if (ip_header->ip_p == 6) {
		packet_tmp->packet_type = Tcp;///YS 包的类型是tcp
		tcp_header = (struct tcphdr *) (packet_tmp->raw_packet + sizeof(struct ether_header) + sizeof(struct ip));

		///YS 数所包的有效长度(TCP有效负载) 也就是TCP的数据长度
		// ip_header->ip_len : Total Length(Header + Data)
		// ip_header->ip_hl : header length
		// tcp_header->th_off ： data offset（可看作是TCP首部长度）
		packet_tmp->packet_size = ntohs(ip_header->ip_len) - (tcp_header->th_off)*4 - (ip_header->ip_hl*4);
		// packet_tmp->packet_size=packet_tmp->cap_size-(tcp_header->th_off)*4 -(ip_header->ip_hl*4)-14;
		//   dbug("packet len is %d\n",packet_tmp->packet_size);

		///YS tcp有效负载的数据的起始位
		packet_tmp->offset = (packet_tmp->cap_size) - packet_tmp->packet_size;
		//packet_tmp->offset = (tcp_header->th_off)*4 +(ip_header->ip_hl*4)+14;
		//dbug("packet header len is %d\n",packet_tmp->offset);

		packet_tmp->src_port = ntohs(tcp_header->th_sport);///YS 源端口
		packet_tmp->dst_port = ntohs(tcp_header->th_dport);
		packet_tmp->ack = ntohl(tcp_header->th_ack);///YS ACK
		packet_tmp->seq = ntohl(tcp_header->th_seq);///YS SEQ
		packet_tmp->flat = tcp_header->th_flags;///YS TCP的FLAG（PUS,ACK,FIN等）

		printf("packet_size = %d, offset = %d\n", packet_tmp->packet_size, packet_tmp->offset);
		printf("Seq = %ld, Ack = %ld, Flag = 0x%02x\n", packet_tmp->seq,
			packet_tmp->ack, packet_tmp->flat);

	///YS 如果是udp包
	} else if (ip_header->ip_p == 17) {
		//dbug("udp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		packet_tmp->packet_type = Udp;///YS包的类型为UDP
		udp = (struct udphdr *) (packet_tmp->raw_packet + sizeof(struct ether_header) + sizeof(struct ip));
		packet_tmp->offset = 8 + (ip_header->ip_hl*4) + 14;///YS 有效数据的起始位
		packet_tmp->packet_size = packet_tmp->cap_size - packet_tmp->offset;///YS 有效数据的总长
		packet_tmp->ip_id = ntohs(ip_header->ip_id);///YS IP头里的ID
		packet_tmp->src_port = ntohs(udp->uh_sport);///YS 源端口
		packet_tmp->dst_port = ntohs(udp->uh_dport);


	} else {
		//dbug("not tcp or udp \n");
		//not tcp or udp ys//
		return -1;///YS 来到这里的包，要么是IPV6，要么是IP报文
	}




	//strcpy(packet_tmp->dst_ip,inet_ntoa(ip_header->ip_dst));
	//ip_header->


	// net_addr 应该是子网名，也就是内网，如192.168
	if (strstr(packet_tmp->dst_ip, net_addr)) {
		packet_tmp->direction = d_in;
		///YS 创建该流的TCP流的名字
		snprintf(packet_tmp->stream, 64, "%s#%d#%s#%d", packet_tmp->dst_ip,
			packet_tmp->dst_port, packet_tmp->src_ip, packet_tmp->src_port);
	} else if(strstr(packet_tmp->src_ip, net_addr)) {
		packet_tmp->direction = d_out;
		memset(packet_tmp->stream, 0, 64);
		///YS 创建该流的TCP流的名字
		snprintf(packet_tmp->stream, 64, "%s#%d#%s#%d", packet_tmp->src_ip,
			packet_tmp->src_port, packet_tmp->dst_ip, packet_tmp->dst_port);
	}

	return 0;
}


/*********************************
计算用户的流量
参数1，packet_tmp，我们的包
**********************************/
int
flow_count(struct packet_info *packet_tmp)
{

	//if(packet_tmp->packet_size>1600) exit(0);
	int ip1, ip2, ip3, ip4;

	if (strstr(packet_tmp->dst_ip, net_addr)) {
		///YS获取IP最后一位，这个位就是数组里的直接应射
		sscanf(packet_tmp->dst_ip, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
		// < 3 ????
		if (strlen(userf[ip4].ip) < 3) {
			sprintf(userf[ip4].ip, "%s", packet_tmp->dst_ip);
			userf[ip4].sub_net = ip3;
			userf[ip4].flow = userf[ip4].flow + packet_tmp->packet_size;///YS 用户的流量增加
			//dbug("11111111111111111111size %d\n",packet_tmp->packet_size);
		} else {
			///YS 如果这个IP已经在我们的流量统计数组里存在了
			userf[ip4].flow = userf[ip4].flow + packet_tmp->packet_size;
			//dbug("222222222222222222size %d  %d  %d\n",packet_tmp->packet_size,userf[ip4].flow,ip4);
		}
		printf("(download) ip: %s, packet size: %d, flow: %ld\n",
			packet_tmp->dst_ip, packet_tmp->packet_size, userf[ip4].flow);
	}


	if (strstr(packet_tmp->src_ip, net_addr)) {
		sscanf(packet_tmp->src_ip, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
		if (strlen(userf[ip4].ip) < 3) {
			sprintf(userf[ip4].ip, "%s", packet_tmp->src_ip);
			userf[ip4].sub_net = ip3;
			userf[ip4].flow = userf[ip4].flow + packet_tmp->packet_size;
			//dbug("33333333333333333333size %d\n",packet_tmp->packet_size);
		} else {
			userf[ip4].flow = userf[ip4].flow + packet_tmp->packet_size;
			//dbug("4444444444444444444444size %d  %d  %d\n",packet_tmp->packet_size,userf[ip4].flow,ip4);
		}
		printf("(upload) ip: %s, packet size: %d, flow: %ld\n",
			packet_tmp->src_ip, packet_tmp->packet_size, userf[ip4].flow);
	}

	return 0;
}




/*********************************
保存数据包，把数据包保存到相应的TCP流结构体里
参数1，要处理的数据包
*********************************/
int store_packet(struct packet_info *packet_tmp2)
{

    struct packet_info *packet_tmp=NULL;
    packet_tmp=packet_tmp2;
    char tcp_stream[64];
    memset(tcp_stream,0,sizeof(tcp_stream));
    struct stream * c;
    c=NULL;
    snprintf(tcp_stream,sizeof(tcp_stream),packet_tmp->stream);
    c=g_hash_table_lookup(tcp_stream_list,tcp_stream);///YS 找一下这个包对应的流是否已经存在了


	printf("store_packet() - tcp_stream = %s\n", tcp_stream);

    // 已经存在
    if(c)
    {
	printf("tcp_stream exist\n");
        //dbug("ddddddd\n");
        c->time=packet_tmp->time;///YS 更新流的时间

        if(packet_tmp->direction)///in
        {
            c->out_effect_len=(int)(packet_tmp->ack - c->out_start);///YS 每个方向的有效长度，是靠返方向的应答包确定的，由应答包的ACK字段确定
            ////dbug("ack  %x  start  %x  >>>%d\n\n",packet_tmp->ack,c->out_start,c->out_effect_len);
        }
        else///out
        {
            c->in_effect_len=packet_tmp->ack-c->in_start;///YS 返方向的,BUFF里有效的长度
        }
        if(packet_tmp->packet_size==0)
		return 0;///YS TCP负载数据为0，没有内容，不用继续走下去


        if(packet_tmp->direction)///方向in
        {
            long tmp_num=packet_tmp->seq - c->in_start - c->in_buff_record;
            //  dbug("111 %x --- %x    \n %ld --- %ld  \n  %ld\n",packet_tmp->seq,c->in_start,packet_tmp->seq-c->in_start,c->in_buff_record  ,tmp_num);
            if(tmp_num>995000)  ///YS 为什么不用对方的ACK？因为ACK有可能远小于 SEQ,ACK=1000000时,seq可能大于1024*1024
            {
                //dbug("imp  seq is %d   %x \n",packet_tmp->seq,packet_tmp->seq);
                //dbug("imp packet_tmp->seq-c->in_start   ===  > %d\n",packet_tmp->seq-c->in_start);
//     dbug("tmp_num   ===  > %d     in_buff_record %d    %d   %s\n",tmp_num,c->in_buff_record,c->in_effect_len,c->stream_index);
                ///YS 先放到文件里，再去HTTP等处理
                int in_count=c->in_effect_len-c->in_buff_record;
                int out_count=c->out_effect_len-c->out_buff_record;
                if(tmp_num>=0 && tmp_num<1100000)
                {
                    memmove(c->istream+tmp_num,    packet_tmp->raw_packet+packet_tmp->offset  ,  packet_tmp->packet_size);
                    create_tmp_file( c,in_count,out_count);///YS ，如果超过995000时，我们就为这个流创建临时文件，创建完成后，就可以清空缓冲区了
                    ///empty buff
                    memset(c->istream,0,in_count);
                    ///memCPY last buff
                    memmove(c->istream,c->istream+in_count,sizeof(c->istream)-in_count);
                    ///empty the last buff
                    memset(c->istream+in_count,0,sizeof(c->istream)-in_count);
                    c->in_buff_record=c->in_effect_len;///YS 每一次写完，我们都需要更新我们的起始位，下次开始时，可以接着写
                }
            }
            if(tmp_num>=0 && tmp_num<1100000)
            {
                //           dbug("move %x --- %x     %d --- %d    %d\n",packet_tmp->seq,c->in_start,packet_tmp->seq-c->in_start,c->in_buff_record  ,tmp_num);
                //          dbug("move2   %d --- %d  \n",packet_tmp->offset ,packet_tmp->packet_size);
                memmove(c->istream+tmp_num   ,    packet_tmp->raw_packet+packet_tmp->offset  ,  packet_tmp->packet_size);
            }
            else
            {
//                dbug("tmp_num wrong  %ld\n",tmp_num);
                // dbug("1111sss %x --- %x    %d --- %d    %d  %s\n",packet_tmp->seq,c->in_start,packet_tmp->seq,c->in_start,tmp_num,c->stream_index);
                // free(c->in_start);
                //  exit(0);
            }
        }
        else///YS 同IN方向一样，只是反方向了
        {
            long tmp_num=0;
            tmp_num=packet_tmp->seq-c->out_start-c->out_buff_record;
            if(tmp_num>995000)
            {
                ///先放到文件里，再去HTTP等处理
                int in_count=c->in_effect_len-c->in_buff_record;
                int out_count=c->out_effect_len-c->out_buff_record;
                if(tmp_num>=0 && tmp_num<1100000)
                {
                    memmove(c->ostream+(tmp_num),packet_tmp->raw_packet+packet_tmp->offset,packet_tmp->packet_size);
                    create_tmp_file( c,in_count,out_count);
                    ///empty buff
                    memset(c->ostream,0,out_count);
                    ///memCPY last buff
                    memmove(c->ostream,c->ostream+out_count,sizeof(c->ostream)-out_count);
                    ///empty the last buff
                    memset(c->ostream+out_count,0,sizeof(c->ostream)-out_count);
                    c->out_buff_record=c->out_effect_len;
                    //            dbug("move2 sucess !!!\n");
                }
            }
            // memmove(c->ostream+(packet_tmp->seq-c->out_start),packet_tmp->raw_packet+packet_tmp->offset,packet_tmp->packet_size);
            if(tmp_num <1100000&&tmp_num>=0)
            {
                memmove(c->ostream+(tmp_num),packet_tmp->raw_packet+packet_tmp->offset,packet_tmp->packet_size);
            }
            else
            {
                dbug("2222 tmp_num wrong   %ld \n",tmp_num);
                //              dbug("2222 stream index %s\n",c->stream_index);
//                dbug("2222 %p --- %p     %ld --- %ld    %ld\n",packet_tmp->seq,c->out_start,packet_tmp->seq-c->out_start,c->out_buff_record  ,tmp_num);
                //    free(c->in_start);
                // exit(0);
            }
        }

    }

////////////////////////////////////////////////////////////////////////////////
    else///YS 如果这个包没有找到他对应的TCP流，那就创建一个TCP流
    {
	printf("tcp_stream not exist\n");

        if(packet_tmp->flat==0x02 )///YS 如果是第一个请求连接包，我们忽略
        {
		printf("first SYN packet\n");
            return 0 ;
        }

				// dbug("insert >>>>> %s\n",packet_tmp->stream);
        //struct stream *stream_tmp=(struct stream *)malloc(sizeof(struct stream));
        struct stream *stream_tmp=calloc(1,sizeof(struct stream));

	// ???????? 为什么要加1
        packet_tmp->seq=packet_tmp->seq+1;///YS 这里要加1，TCP的流SEQ是以+1开始的

        if(strstr(packet_tmp->dst_ip,net_addr))///YS 给流的IP赋值
        {// in
            strcpy(stream_tmp->user_ip,packet_tmp->dst_ip);
            strcpy(stream_tmp->dst_ip,packet_tmp->src_ip);
        }
        else
        {// out
            if(strstr(packet_tmp->src_ip,net_addr))
            {
                strcpy(stream_tmp->user_ip,packet_tmp->src_ip);
                strcpy(stream_tmp->dst_ip,packet_tmp->dst_ip);
            }
        }

        snprintf(stream_tmp->stream_index,64,"%s",packet_tmp->stream);///YS 流的索引名
        stream_tmp->pro_header_len=0;///YS 协议头长
        stream_tmp->file_offset=-1;///YS 文件的对应文件写到哪里
        stream_tmp->already_process=0;///YS 是否被处理过
        stream_tmp->in_buff_record=0;///YS IN方向的起始位记录
        stream_tmp->in_start=0;///YS IN方向开始的序列号
        stream_tmp->out_start=0;///YS OUT方向开始的序列号
        snprintf(stream_tmp->pro,32,"%s",packet_tmp->behavior_type);  ///YS 流的协议名
        if(packet_tmp->direction)///YS 序列号的获取
        {// in
            stream_tmp->in_start=packet_tmp->seq;
             stream_tmp->out_start=packet_tmp->ack;
        }
        else
        {// out
            stream_tmp->out_start=packet_tmp->seq;
            stream_tmp->in_start=packet_tmp->ack;
        }

        //dbug("11111>>>>. insert to hash list %s   instart :%x   outstart  %x\n",tcp_stream,stream_tmp->in_start,stream_tmp->out_start);
        g_hash_table_insert(tcp_stream_list, g_strdup(tcp_stream), stream_tmp);///YS 把流的指针插入到哈希表中
        mallco_num++;
        //dbug("hash list insert count  %d\n",mallco_num);
        fflush(stdout);
    }


    return 0;
}


int remove_stream(struct packet_info *packet_tmp)
{
    char tcp_stream[64];
    memset(tcp_stream,0,64);
    snprintf(tcp_stream,64,"%s",packet_tmp->stream);
    ///ulink!!!
    struct stream * c;
    c=NULL;
    c=g_hash_table_lookup(tcp_stream_list, tcp_stream);
    // 存在
    if(c)
    {
        int in_count=c->in_effect_len-c->in_buff_record;
        int out_count=c->out_effect_len-c->out_buff_record;
        if(in_count>0||out_count>0)
        {
            //dbug("remove it %s   inlen is %d  in_effect_len  is %d \n",c->stream_index,in_count,c->in_effect_len);
            create_tmp_file( c,in_count,out_count);
            process_stream(c);
            //if(ex){
				//printf("in remove_stream\n");
				//exit(0);
		//		}
        }
        if(g_hash_table_remove(tcp_stream_list,tcp_stream))
        {
            //dbug("g_hash_table_remove2  success !!\n");
        }
    }
    return 0;
}


/**********************
处理TCP流
参数1，是我们的自定义的TCP流结构体
***********************/
int process_stream(struct stream * s)
{
    char tmp_file_name_in[1024];
    char tmp_file_name_out[1024];
    snprintf(tmp_file_name_in,sizeof(tmp_file_name_in),"%s_in",s->stream_index);
    snprintf(tmp_file_name_out,sizeof(tmp_file_name_out),"%s_out",s->stream_index);
    if(!strcmp(s->pro,"http"))
    {
        process_out_stream(tmp_file_name_in,tmp_file_name_out,s->user_ip);///YS 名字虽然是”process_out_stream“，但是这个函数会，处理我们的流，包括进和出的
    }
    if(!strcmp(s->pro,"smtp"))
    {
        dbug(" SMTP##################################\n")  ;

        //smtp(tmp_file_name_out,tmp_file_name_in,s->user_ip);///YS sendemail,so we use out_strema

    }
    if(!strcmp(s->pro,"pop3"))
    {
        dbug(" pop3##################################\n")  ;
        //pop3(tmp_file_name_in,s->user_ip);
    }

    unlink(tmp_file_name_in);///YS 删除临时文件
    unlink(tmp_file_name_out);///YS 删除临时文件
    return 0;
}



/****************************
处理我们的TCP流，包括IN,OUT两个方向的流
参数1，IN—STREAM的字符，如192.168.1.12999221.23.11.2280_in
参数2，OUT-STREAM的字符，如192.168.1.12999221.23.11.2280_out
参数3，用户的IP
****************************/

int process_out_stream(char *in_file,char *out_file,char *ip)
{
    int i=0,ret=0;
    struct http_info http_info_tmp;
    char *p=NULL,*p2=NULL;
    char *out_file_content=NULL;
    char *in_file_content=NULL;
    long in_file_len=0;
    long out_file_len=0;
    ///for(){process   out/r/n}//get ,post
    get_file_content (out_file,&out_file_len,&out_file_content);
    get_file_content (in_file,&in_file_len,&in_file_content);

		///YS  提前处理我们的内容
    //process_email_post(out_file_content,out_file_len,ip,0);///YS 处理EMAIL，处理的是POST的，即发邮件的
    //http_search(out_file_content,out_file_len,ip);///YS 处理搜索
    //process_bbs(out_file_content,out_file_len,ip);///YS 处理BBS
    if(out_file_len)
    {
        for(i=0; i<100; i++)///YS 一个流里面可能会有多个HTTP，如KEEPALIVE，这个FOR语句仅针对HTTP
        {
            p=(char *)memmem( out_file_content,out_file_len,"GET ",4);
            if(p)
            {
                ///YS 找到HTTP头了
                p=(char *)memmem( out_file_content,out_file_len,"\x0D\x0A\x0D\x0A",4 );
                if(p)
                {
                    http_info_tmp.http_hl=(int)(p-out_file_content)+4;
                    char *header_buff=  (char*) malloc (sizeof(char)*http_info_tmp.http_hl);
                    memmove(header_buff,out_file_content,http_info_tmp.http_hl);///YS 处理完的信息可以不要了，这里使用MOVE直接覆盖掉，下面的情况也是
                    process_http_header(header_buff, &http_info_tmp,2,ip);///YS 处理HTTP头
/*
                    if((ret=process_email_get(header_buff,http_info_tmp.http_hl)))///YS 处理EMAIL，GET，即收邮件
                    {
                        read_email_html(in_file_content,ip,in_file_len,ret-1);

                    }
*/
                    ///YS 清空HTTP头
                    memset(out_file_content,0,http_info_tmp.http_hl);
                    memmove(out_file_content,out_file_content+http_info_tmp.http_hl,out_file_len-http_info_tmp.http_hl);
                    memset(out_file_content+out_file_len-http_info_tmp.http_hl,0,http_info_tmp.http_hl);

                    free(header_buff);
                    ///YS 处理完GET，对应的HTTP 200，304也要处理
                    if(in_file_len>0)
                    {
			///YS 这里的INstream,是对应用的GET，返回来的内容
                        process_in_stream(in_file_content,http_info_tmp.filename,in_file_len,ip);
                    }
                    continue;
                }
            }
            else///YS 上面是处理GET的，下面是处理POST
            {
		//dbug("for POST !!!!\n");

                p=(char *)memmem( out_file_content,out_file_len,"POST ",5 );
                if(p)
                {
                    p=(char *)memmem( out_file_content,out_file_len,"\x48\x54\x54\x50\x2F\x31\x2E\x31\x0D\x0A",10 ); ///YS 同样的，POST也会有KEEPLIVE情况，一个流里存在多个HTTP头，那一串16进制是HTTP/1.1\R\N
                    if(p)///YS 找到HTTP头，POST的头
                    {
                        process_email_post(out_file_content,out_file_len,ip,0);///处理EMAIL POST，发邮件不过前面好像已经处理过了，有代测试
                        p2=(char *)memmem( p+10,out_file_len,"\x48\x54\x54\x50\x2F\x31\x2E\x31\x0D\x0A",10 ); //HTTP/1.1\R\N
                        if(p2)///YS 找到第二个HTTP头
                        {
                            dbug("goto  upload_file_name  function %ld\n",p2-p);
                            ///YS 确定写入的长度，到第二个HTTP1.1前，用于分割多个HTTP信息
                            int len=p2-p;
                            upload_file_name(out_file_content,len,ip);///YS 这个方法是用正则式的，通用性强，不过文件名，跟长度都是有一定的误差，不过可以接受。下面这个注释了的代码是可以运行的，是针对特定的上传方式boundary才用的。
                            memset(out_file_content,0,len);
                            memmove(out_file_content,out_file_content+len,out_file_len-len);
                            memset(out_file_content+out_file_len-len,0,len);
                        }
                        else
                        {
                            ///YS 没有找到第二个HTTP头
                            upload_file_name(out_file_content,out_file_len,ip);///这个方法是用正则式的，通用性强，不过文件名，跟长度都是有一定的误差，不过可以接受。下面这个注释了的代码是可以运行的，是针对特定的上传方式boundary才用的。
                            memset(out_file_content,0,out_file_len);
                        }


                    }

                    /*
                    p=(char *)memmem( out_file_content,out_file_len,"\x0D\x0A\x0D\x0A",4 );///找到HTTP头了
                    if(p)
                    {
                        http_info_tmp.http_hl=(int)(p-out_file_content)+4;
                        char *header_buff=  (char*) malloc (sizeof(char)*http_info_tmp.http_hl);
                        memmove(header_buff,out_file_content,http_info_tmp.http_hl);
                        ret=process_http_header(header_buff, &http_info_tmp,3);
                        ///清空HTTP头
                        memset(out_file_content,0,http_info_tmp.http_hl);
                        memmove(out_file_content,out_file_content+http_info_tmp.http_hl,out_file_len-http_info_tmp.http_hl);
                        memset(out_file_content+out_file_len-http_info_tmp.http_hl,0,http_info_tmp.http_hl);
                        free(header_buff);
                        if(ret==3)///boundary
                        {
                            process_post_boundary(out_file_content,out_file_len,ip);
                        }*/


                    continue;
                }
                else
                {
                    break;
                }
            }
        }
    }
    free(out_file_content);
    free(in_file_content);
    in_file_content=NULL;
    out_file_content=NULL;
    return 0;
}



/************************
把临时文件从硬盘里读到内存
参数1，是文件名
参数2，是文件的长度
参数3，是文件读入内存后所在的指针
*************************/
char *get_file_content (char *filename,long *len,char **buffer)
{
    FILE * pFile;
    long lSize;
    size_t result;
    ///YS 若要一个byte不漏地读入整个文件，只能采用二进制方式打开
    pFile = fopen (filename, "rb" );
    if (pFile==NULL)
    {
//        dbug ("the filename is %s   's len is NULL??\n",filename);
        return 0;
        //exit (1);
    }

    ///YS  获取文件大小
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    ///YS  分配内存存储整个文件

    *buffer = calloc (lSize,sizeof(char));
    if (*buffer == NULL)
    {
        fputs ("Memory error",stderr);
       // exit (2);
    }

    ///YS 将文件拷贝到buffer中
    result = fread (*buffer,1,lSize,pFile);
    if (result != lSize)
    {
        fputs ("Reading error",stderr);
     //   exit (3);
    }
    fclose (pFile);
    *len=lSize;///YS 返回长度
return 0;
}


/**************************
处理HTTP头
参数1，HTTP头的BUFF指针
参数2，http的结构体
参数3，TCP流的方向
参数4，用户IP
***************************/
int  process_http_header(const char *header_buff,   struct http_info * http_info_tmp,int direct ,char *ip)
{
    if(direct==1)///YS 200，304等这些都是IN方向才会有的
    {
        char *p;
        p=(char*)memmem( header_buff,http_info_tmp->http_hl,"304 Not Modified",14 );
        if(p)
        {
            //   dbug(">>>>>>>>>>>>>>>>>>>>>>>>3 0 4\n");
            return  304;
        }
        else
        {
            p=(char*)memmem( header_buff,http_info_tmp->http_hl,"200 OK",6 );
            if(p)
            {
                //    dbug(">>>>>>>>>>>>>>>>>>>>>>>>2 0 0 ok\n");
                return 200;
            }

            p=(char*)memmem( header_buff,http_info_tmp->http_hl,"206 Par",6 );
            if(p)
            {
                //     dbug(">>>>>>>>>>>>>>>>>>>>>>>>206 Par\n");
                return 206;
            }
        }
    }
    if(direct==2)///如果是OUT方向的，我们就找HOST和REFERER，和用户GET了什么文件
    {
        char *p,*p2;
        int len_start,len_end;
        ///YS 获取URL
        p=(char *)memmem( header_buff,http_info_tmp->http_hl,"Host: ",6 );
        if(p)
        {
            len_start=p-header_buff;
            p2=strchr(header_buff+len_start,'\r');
            if(p2)
            {
                //len_end=p2-p+1;
                len_end=p2-p;

                if(len_end-5>sizeof(http_info_tmp->url))
                {
                    snprintf(http_info_tmp->url,sizeof(http_info_tmp->url),"%s",header_buff+len_start+6);
                }
                else
                {
                    snprintf(http_info_tmp->url,len_end-5,"%s",header_buff+len_start+6);
                }


                if(http_enable)
                {
                    ///YS get the referer
                    p=(char *)memmem( header_buff,http_info_tmp->http_hl,"Referer:",8 );
                    if(p)
                    {
                        len_start=p-header_buff;
                        p2=strchr(header_buff+len_start,'\r');
                        if(p2)
                        {
                            // len_end=p2-p+1;
                            len_end=p2-p;

                            if(len_end-13>sizeof(http_info_tmp->referer))
                            {
                                snprintf(http_info_tmp->referer,sizeof(http_info_tmp->referer),"%s",header_buff+len_start+16);
                            }
                            else
                            {
                                snprintf(http_info_tmp->referer,len_end-15,"%s",header_buff+len_start+16);
                            }

                        }
                        //dbug(" %s\n",http_info_tmp->url );
                        //dbug(" %s\n",http_info_tmp->referer );
                        if(strlen(http_info_tmp->referer)>5&& strlen(http_info_tmp->url)>5)
                        {
 //                           dbug("%s   %s\n",http_info_tmp->url,http_info_tmp->referer);
                            put_http_host_into_db(ip,http_info_tmp->url,http_info_tmp->referer);///YS 把URL，REFERE写入数据库
                        }
                    }
                }
            }

            ///YS 如果找到GET和HTTP字眼，那就说明用户在浏览或者下载
            p=strstr(header_buff,"GET ");
            if(p)
            {
                len_start=p-header_buff;
                p2=strstr(header_buff+len_start," HTTP");
                if(p2)
                {
                    char str[2024];
                    if(p2-p>sizeof(str))
                    {
                        snprintf(str,sizeof(str),"%s",header_buff+len_start+4 );
                    }
                    else
                    {
                        snprintf(str,p2-p-3,"%s",header_buff+len_start+4 );
                    }

                    strcat(http_info_tmp->url,str);
                }
            }
            ///YS get filename ，如http://www.xxx.x.x.com/aaa.doc，从后面数起，遇到第一个/为止
            p=strrchr(http_info_tmp->url,'/');
            if(p)
            {
                snprintf(http_info_tmp->filename,512,"%s",p+1);
            }
        }
    }

    if(direct==3)///YS for out strem  POST主要是拿文件名和结束
    {
        char *p,*p2;
        int len_start;
        ///get url
        p=(char *)memmem( header_buff,http_info_tmp->http_hl,"boundary=",9 );///YS boundary，上传时的特征
        if(p)
        {
            len_start=p-header_buff;
            p2=strchr(header_buff+len_start,'\r');
            if(p2)
            {
                return 3;
            }
        }
    }

    return  0;
}


/***********************
处理IN—STREAM的函数
参数1，TCP流INSTREAM的BUFF
参数2，文件名
参数3，BUFF的长度
参数4，用户IP
***********************/
/*
int process_in_stream(char * in_file_content,char  *filename,long in_file_len,char *ip)
{
    struct http_info http_info_tmp;
    http_info_tmp.file_len=0;
    char *p=NULL;
    char *p2=NULL;
    int res=0;
    ///for(){process   out/r/n}//get ,post
    if(in_file_len)
    {
        snprintf(http_info_tmp.filename,512,"%s",filename);
        p=(char *)memmem( in_file_content,in_file_len,"HTTP/1.1",8);
        if(p)
        {
            ///YS 找到HTTP头了
            p=(char *)memmem( in_file_content,in_file_len,"\x0D\x0A\x0D\x0A",4 );
            if(p)
            {
                http_info_tmp.http_hl=(int)(p-in_file_content)+4;
                char *header_buff=NULL;
                header_buff=malloc (sizeof(char)*http_info_tmp.http_hl);
                memmove(header_buff,in_file_content,http_info_tmp.http_hl);
                res=process_http_header(header_buff, &http_info_tmp,1,ip);///YS 处理HTTP头

                ///if(有用的话写文件)
                int len=0;
                if(res==200 || res==206)///YS 304那些都是没有内容的，只有200跟206才会走进来
                {
                    //dbug("get_http_download_filename_and_offset   start !!!!\n");
                    get_http_download_filename_and_offset(header_buff, http_info_tmp.http_hl,&http_info_tmp);///YS 看看有没有偏移量
                    if(http_info_tmp.file_offset)
                    {
                        //        dbug("7777777  have offset ,offset is: %d\n",http_info_tmp.file_offset);
                        len=in_file_len-http_info_tmp.http_hl;
                        if(len>0)
                        {
                            //                dbug("len is %d    name is %s    filelen is %d\n",len,http_info_tmp.filename,http_info_tmp.file_len);
                            ///YS 创建文件，带偏移量的
                            write_file_new(http_info_tmp.filename,len,http_info_tmp.file_offset,in_file_content+http_info_tmp.http_hl,http_info_tmp.file_len,ip,1);
                        }
                        //        dbug("7777777  end!!!1\n");
                        if(len+http_info_tmp.http_hl<in_file_len)
                        {
                            // dbug("memset file ready!\n");
                            memset(in_file_content,0,len+http_info_tmp.http_hl);
                            // dbug("memset file success!\n");
                        }
                        p2=NULL;
                        p=NULL;
                    }
                    else///YS 创建文件，不带偏移量的
                    {
                        ///YS write file until “HTTP/1.1”

                        p2=(char *)memmem( in_file_content+http_info_tmp.http_hl,in_file_len-http_info_tmp.http_hl,"HTTP/1.1",8);
                        if(p2)
                        {
                            len = p2-p-4; ///GET THE LEN
                            //       dbug(" 9999999999 %s  len is %d   %d\n",http_info_tmp.filename,len,http_info_tmp.http_hl);
                            if(len>0)write_file_new(http_info_tmp.filename,len,0,in_file_content+http_info_tmp.http_hl,http_info_tmp.file_len,ip,1);
                            if(len+http_info_tmp.http_hl<in_file_len)
                            {
                                memset(in_file_content,0,len+http_info_tmp.http_hl);
                            }
                            p2=NULL;
                            p=NULL;
                            //memset(in_file_content,0,temp_len);
                        }
                        else
                        {
                            p2=(char *)memmem(in_file_content,in_file_len,"--------",8);///HTTP 特征，一时忘记了，后补
                            if(p2)
                            {
                                len=p2-p+4;
                            }
                            else
                                len=in_file_len-http_info_tmp.http_hl;
                            //          dbug("8888888888 888888888888  %s  len is %d \n",http_info_tmp.filename,len);
                            if(len>0)
                            {
                                //dbug("write file ready!\n");
                                write_file_new(http_info_tmp.filename,len,0,in_file_content+http_info_tmp.http_hl,http_info_tmp.file_len,ip,1);
                                // dbug("write file success!\n");
                            }
                            if(len+http_info_tmp.http_hl<in_file_len)
                            {
                                // dbug("memset file ready!\n");
                                memset(in_file_content,0,len+http_info_tmp.http_hl);
                                // dbug("memset file success!\n");
                            }
                            p2=NULL;
                            p=NULL;
                        }

                    }

                }

                //memset(in_file_content,0,len+http_info_tmp.http_hl);
                if((len>0))
                {
                	  ///YS 清空HTTP头
                    //dbug("move file ready!\n");
                    memmove(in_file_content,in_file_content+len+http_info_tmp.http_hl,in_file_len-len-http_info_tmp.http_hl);
                    memset(in_file_content+in_file_len-len-http_info_tmp.http_hl,'-',len+http_info_tmp.http_hl);
                    //dbug("move file success!\n");
                }
                free(header_buff);
                // free(header_buff);
            }
        }
    }
    return 0;
}
*/









