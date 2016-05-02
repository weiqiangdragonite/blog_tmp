/*
 *
 */

#ifndef PACKET_INFO_H
#define PACKET_INFO_H

#include <sys/types.h>
#include <glib.h>
#include <pcre.h>


/*
 *
 */
struct behavior_type {
	int type;
	int behavior;
	char detail[64];
	int already_process;
};
typedef struct behavior_type BEHAVIOR_TYPE;


/*
 * 抓到的包要分别进行审计和拦截
 */
struct packet_info {
	u_int8_t raw_packet[65535];///YS 源始数据包
	//char http[1500];///YS 数据包的有效信息
	char gid[8];///YS 用户组ID
	int packet_id;///YS 数据包顺序号
	int packet_size;///YS 包的有效长度，除去包头
	int cap_size;///YS 抓包抓到的实际长度
	struct timeval cap_time;///YS 数据包的抓包时间
	BEHAVIOR_TYPE behavior;///YS 数据包的行为类型
	char behavior_type[32];///YS 协议名
	u_short src_port;///YS 源端口
	u_short dst_port;///YS 目标端口
	u_short ip_id;///YS IP报文的ID
	char src_ip[32];///YS 源IP
	char dst_ip[32];///YS 目标IP
	u_char src_mac[6];///YS 源MAC
	u_char dst_mac[6];///YS 目标MAC
	u_short win;///YS 数据包窗口大小
	unsigned long int seq;///YS 数据包的序列号
	unsigned long int ack;///YS 数据包的确认号
	int block_flag;///YS 用来区分是TCP拦截还是 HTTP拦截
	int packet_type;///YS 1 for tcp ,0for udp
	int direction;///YS  方向 1  in  |||  0 out
	char stream[64];///YS TCP流的名字
	int offset;///YS 数据包包头的偏移，过了这个偏移，就是数据包的有效数据
	int flat;///YS 这个在审计里用不上
	int time;
};
typedef struct packet_info ElemType;



///YS 用户流量信息的结构体
typedef struct user_flow
{
    long flow;///YS 用户流量
    char ip[32];///YS 用户IP
    int  sub_net;///YS 用户所在的子网
    long limit;///YS 用户的流量上限
} S_user_flow;



/*****************
  数据包特征字
*****************/
typedef struct pcre_info
{
	char name[32];///YS 特征字对应的协议
	char feature[128];///YS 特征字
	pcre *p_pcre;///YS 编译后的正式指针
} S_pcre;

///YS 我们自定义的流的结构体
typedef struct stream
{
    char stream_index[64]; ///YS 每一个流的索引都使用该名字：ip+prot+ip+prot


    unsigned long int in_start;///YS IN方向起始的序列号
    long in_effect_len;///YS IN方向的有效长度
    unsigned long int in_ack;///YS IN方向的确认号
    long in_buff_record;///YS IN方向写到哪里，可以看作为istream[1024*1024]这个长长的BUFF的游标

    unsigned long int out_start;///YS OUT方向起始的序列号
    long out_buff_record;///YS out方向写到哪里，可以看作为istream[1024*1024]这个长长的BUFF的游标
    long out_effect_len;///YS OUT方向的有效长度
    unsigned long int out_ack;///YS OUT方向的确认号

    char file_name[512];///YS 这个流里涉及到的文件名
    int file_len;///YS 文件长度
    long file_offset;///YS 文件的偏移量
    int pro_header_len;///YS 像TFTP那种，会有一个固定的长度
    int write_to_file;///YS 是否写入文件
    char ostream[1024*1024];///YS 4120 KB ，Out方向的流的缓冲
    char istream[1024*1024];///YS 4120 KB ，In方向的流的缓冲
    char pro[32];///YS 协议
    time_t time;///YS 时间戳，可以作为该流是否已经传送完成
    char user_ip[24];///YS 用户IP
    char dst_ip[24];///YS 目标IP
    int last_file_len;///YS 暂时没有使用
    int already_process;///YS 是否被处理过，处理过的话，有一些变量已经不再需要重新开始取值
} S_stream;


///YS HTTP_INFO 结构体
typedef struct http_info
{
    int http_hl;///YS http头部信息的长度
    int type;///YS 1:get  2:post  3:head
    char filename[512];///YS 文件名
    char url[2024];///YS URL
    char user_ip[24];///YS 用户IP
    char dst_ip[24];///YS 目标IP
    char  time[32];///YS 时间戳
    long file_len;///YS 文件长度
    long file_offset;///YS 文件的偏移量
    char content_length[512];///YS 内容的长度
    char content_range[512];///YS 内容的起始偏移
    char attache_filename[1440];///YS 是否由attache产生的filename
    char referer[1440];///YS refererURL

} S_http;


#endif	/* PACKET_INFO_H */
