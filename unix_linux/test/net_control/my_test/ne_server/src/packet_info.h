///YS 我们自定义的一些结构体都在这里

#ifndef PACKET_INFO_H
#define PACKET_INFO_H


#include <pcre.h>
#include <sys/types.h>
#include <mysql.h>
#include <glib.h>

/*****************
  数据包的行为类型
*****************/
typedef struct
{
    int type; ///YS 数据包类型 1 is whitelist ,2 person list,3 group list,4 whole list
    int behavior;///YS URL_adult,URL_new,APP_im
    char detail[64];///YS www.baidu.com,YY,QQ
    int already_process;
} BEHAVIOR_TYPE;



/*****************
  数据包信息
*****************/
typedef struct packet_info
{
    u_int8_t  raw_packet[64]; ///YS 源始数据包的包头
    char http[1500];  ///YS 数据包的有效信息
    char gid[8];	///YS 用户组ID
    int packet_id; ///YS 数据包顺序号
    int packet_size;///YS 包的有效长度，除去包头
    int cap_size; ///YS 抓包抓到的实际长度
    struct timeval cap_time;///YS 数据包的抓包时间
    BEHAVIOR_TYPE behavior_type;///YS 数据包的行为类型
    u_short src_port; ///YS 源端口
    u_short dst_port;///YS 目标端口
    char src_ip[32];///YS 源IP
    char dst_ip[32];///YS 目标IP
    u_char src_mac[6];///YS 源MAC
    u_char dst_mac[6];///YS 目标MAC
    u_short win; ///YS 数据包窗口大小
    unsigned long int seq;///YS 数据包的序列号
    unsigned long int ack;///YS 数据包的确认号
    int block_flag;///YS 用来区分是TCP拦截还是 HTTP拦截
} ElemType;


/*****************
  数据包特征字
*****************/
typedef struct pcre_info
{
    char name[32];///YS 特征字对应的协议
    char feature[128];///YS 特征字
    pcre *p_pcre;///YS 编译后的正式指针
} S_pcre;


/*****************
  服务器IP与应用名字
*****************/
typedef struct app_server_ip
{
    char name[32];///YS 服务器ID对应的应用名字
    char ip[32];///YS 服务器IP
} S_app_server_ip;


/*****************
  arp用户
*****************/
typedef struct arp_user
{
    char arp_packet[4];
    char ip[32]; ///YS IP用户
    u_int8_t mac[128];///YS 对应的用户的MAC
    int count; ///YS 累计发送的APR包数
    int flag;///YS 是否需要存放到ARP_USER_LIST
    int from;///YS from==4时，那就是OVER FLOW的用户
} S_arp_user;


/*****************
  PHP传过来的CMD，暂时没有用
*****************/
typedef struct cmd_info
{
    int cmd_type;
    char key[128];
    GHashTable *p_hash_table;
    int enable;
} S_cmd;


/*****************
  发送到审计端到包
*****************/
typedef struct sock_buff
{
    int len;///YS 源始数据包的长度
    unsigned char raw_packet[1600]; ///YS 保存源始数据包
} S_sock_buff;


#endif
