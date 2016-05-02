///YS 我们自定义的数据，结构体通常放在这里

#ifndef PACKET_INFO_H
#define PACKET_INFO_H
#include <pcre.h>
#include <sys/types.h>
#include <time.h>

///YS TCPFLAG的值
#define	TH_FIN	0x01
#define	TH_SYN	0x02
#define	TH_RST	0x04
#define	TH_PUSH	0x08
#define	TH_ACK	0x10
#define	TH_URG	0x20
#define	TH_ECE	0x40
#define	TH_CWR	0x80

///YS 正则式结构体
typedef struct pcre_info
{
    char name[32];///YS 协议名字
    char feature[128]; ///YS 协议的特征
    pcre *p_pcre;///YS 协议的正则式编译后的模板
} S_pcre;


///YS 用户流量信息的结构体
typedef struct user_flow
{
    long flow;///YS 用户流量
    char ip[32];///YS 用户IP
    int    sub_net;///YS 用户所在的子网
    long limit;///YS 用户的流量上限
} S_user_flow;


///YS 有的协议是在传文件时，前面有部分数据是没用的，如TFTP
typedef struct pro_header_offset
{
    char pro_name[32];///YS 协议名字
    char offset_feature[16];///YS 偏移量的特征
    int offset_type;///YS 偏移类型，是固定的长度，还是以特征字为准
    int offset_len;///YS 偏移的固定长度
} S_pro_header_offset;

///YS 聊天应用的结构体
typedef struct pcre_info_im
{
    char name[32];///YS 协议名字
    char feature[128];///YS 协议特征
    int case_value;///YS ID
    char content_feature_s[32];///YS 内容字段的开始的特征
    char content_feature_e[32];///YS 内容字段的结束的特征
    char sender_s[32];
    char sender_e[32];
    char receiver_s[64];
    char receiver_e[64];
    char login_user_s[64];
    char login_user_e[64];
    char login_user_ex[64];///YS 这种带EX的变量，是一定要有这个字眼才算是找到了
    char user_name[64];
    char content_feature_ex[64];
    char receiver_ex[64];
    char sender_ex[64];
    char in_content_feature_s[64];
    char in_content_feature_e[64];
    char in_sender_s[64];
    char in_sender_e[64];
    char in_receiver_s[64];
    char in_receiver_e[64];
    char in_ex[64];///YS 这个是我们收到聊天信息的特征字
    char ex_1[64];

} S_pcre_info_im;




typedef struct
{
    int flag;
    unsigned long int seq_start;
    unsigned long int seq_end;
} sack_info;


/*typedef struct stream
{
     sack_info sack_info_array[6];//6 maybe a bug
    int use;
    int last_sack_len;
    char stream_index[64]; //steam  index ,ip+prot+ip+prot
    unsigned long int in_seq;
    unsigned long int in_next_seq;
    unsigned long int in_next_seq2;
    unsigned long int in_ack;
    unsigned long int out_seq;
    unsigned long int out_next_seq;
    unsigned long int out_next_seq2;
    unsigned long int out_ack;
    char file_name[64];
    int len;
    int file_offset;
    char len_sign[32];
    char ostream[1024*2120];//4120 K B
    char istream[1024*2120];//4120KB
    int in_offset;
    int out_offset;
    char pro[32];//HTTP FTP  TFTP
    int time;
}S_stream;
*/


///YS BBS应用的结构体，跟聊天应用差不多
typedef struct bbs_info
{
    int case_value;
    char host[64];
    char host_1[32];///YS 有的BBS是二级域名的型式出现的
    char host_2[32];
    int host_url_type;
    char post_url_feature_s[32];
    char post_url_feature_s_2[32];
    char post_url_feature_e[32];
    char post_url_feature_e_2[32];
    char content_feature_s[32];
    char content_feature_e[32];
    char content_feature_s_2[32];
    char content_feature_e_2[32];
    char poster_s[32];
    char poster_e[32];
    char poster_s_2[32];
    char poster_e_2[32];
    char title_s[32];
    char title_e[32];
    char title_s_2[32];
    char title_e_2[32];
} S_bbs;


/*****************
  arp用户
*****************/
typedef struct arp_user
{
    char arp_packet[4];
    char ip[32]; ///YS IP用户
    u_int8_t mac[6];///YS 对应的用户的MAC
    int count; ///YS 累计发送的APR包数
    int flag;///YS 是否需要存放到ARP_USER_LIST
    int from;///YS from==4时，那就是OVER FLOW的用户
} S_arp_user;



///YS 搜索应用的结构体，跟聊天应用差不多
typedef struct search_info
{
    int case_value;
    char host[64];
    char key_word_s[32];
    char key_word_e[32];
    char key_word_s_2[32];
    char key_word_e_2[32];
    char key_word_s_3[32];
    char key_word_e_3[32];
    char auto_search[32];///YS 搜索时的自动提示
} S_search;

///YS 邮件发送应用的结构体，跟聊天应用差不多
typedef struct email_info
{
    int case_value;
    char host[64];
    char content_feature_s[32];
    char content_feature_e[32];
    char content_feature_s_2[32];
    char content_feature_e_2[32];
    char sender_s[32];
    char sender_e[32];
    char sender_s_2[32];
    char sender_e_2[32];

    char receiver_s[64];
    char receiver_e[64];
    char receiver_s_2[64];
    char receiver_e_2[64];

    char title_s[32];
    char title_e[32];
    char title_s_2[32];
    char title_e_2[32];
} S_email;



///YS 邮件浏览用的结构体，跟聊天应用差不多
typedef struct email_html_info
{
    int case_value;
    char host[64];
    char get_feature[244];
    char http_respont_feature[244];
    char save_flag_1[244];
    char save_flag_2[244];

    int pop3;
    char content_s[244];
    char content_e[244];
    char sender_s[244];
    char sender_e[244];
    char receiver_s[244];
    char receiver_e[244];
    char title_s[244];
    char title_e[244];
    char title_e2[244];
    int gzip;///YS 是否是GZIP压缩了，实际上的话，就算是同一个邮件系统，也不一定全是被压缩了，所以这个值设置了，还需要再判断
} S_email_html;


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




///YS 文件类型结构体
typedef struct file_type
{
    char filetype[8];///YS .JPG  .RAR .ZIP  等等
    char filetype_header[256];///YS 文件的文件头
    pcre *p_pcre;///YS 对应的文件的文件头的正则式编译后的模板
} S_file_type;





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



///YS SOCKET接收的数据包
typedef struct sock_buff
{
    int len;///YS 长度
    unsigned char raw_packet[1600];///YS 原始数据包
} S_sock_buff;


///YS 我们自定义的包
typedef struct packet_info
{
    int time;  ///YS 这个包的时间戳
    unsigned char  raw_packet[1520];//the true len is 1514;
    char gid[8]; ///YS 组ID
    int flat;///YS 这个在审计里用不上
    int direction;///YS  方向 1  in  |||  0 out
    int packet_size;///YS 数据包的有效负载长度
    int cap_size;///YS 数据包的整长
    struct timeval cap_time;///YS 抓到包的时间
    char behavior_type[32];///YS 协议名
    u_short src_port;///YS 源端口名
    u_short dst_port;///YS 目标名
    u_short ip_id;///YS IP报文的ID
    char src_ip[32];///YS 原IP
    char dst_ip[32];///YS 目标IP
    unsigned long int seq;///YS SEQ
    unsigned long int ack;///YS ACK
    char stream[64];///YS TCP流的名字
    int packet_type;///YS 1 for tcp ,0for udp
    int offset;///YS 数据包包头的偏移，过了这个偏移，就是数据包的有效数据
    //int tcp_options;//YS 1 for SACK
    //int gid;
    //struct HTTP_HEADER http_header;
} ElemType;



#endif
