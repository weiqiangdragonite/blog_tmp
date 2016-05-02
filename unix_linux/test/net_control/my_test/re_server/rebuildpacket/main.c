#include <stdio.h>
#include "packet_info.h"
#include "ringbuff.h"
#include "config.h"
#include <pcap/pcap.h>
#include <glib.h>
#include <mysql.h>
#include <string.h>
#include <pcap/pcap.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <glib.h>
#include "TCP_funtion.h"
#include <pthread.h>
#include <time.h>



#define arp_flag 3 ///ys 获取ARP时的参数之一
CircularBuffer cb_rawpacket; ///YS 用于保存从SOCKET读到的数据包
char filename_type[1024];///YS 这个值是用于保存文件类型都有哪些，实际的情况会像：“.doc.jpg.rar.zip.ppt.....”等等
GThreadPool *pool_rawpacket_process = NULL;///YS 本来找算用线程池的，但是因为线程池的多线程是处于无序状态，如果10个线程同时去处理数据包，这样本来按序过来的包，可能因此乱了，所以，TCP重组前不能使用。
GError *error = NULL;
extern struct pcre_info s_pcre[pcre_num];///YS 正则式结构体
extern struct pcre_info_im app_im_pcre[pcre_num];///YS 聊天应的正则式结构体
extern int cache_count(void);///YS 定时任务，把超时的流拿出来，去审计
extern int sql_get_flow(char *SELECT_QUERY,MYSQL *sock);///YS 获取用户的流量
extern int sql_get_file_name(char *SELECT_QUERY,MYSQL *sock);///YS 获取要过滤的文件类型（.JPG  .RAR .ZIP）
extern int sql_start(MYSQL *mysql);///YS 初始MYSQL
extern int sql_get_configure_data(char *SELECT_QUERY,MYSQL *sock,void * data,int data_type,int data_len);///YS 初始系统的配置
extern int sql_get_file_name_feature(char *SELECT_QUERY,MYSQL *sock);///YS 获取要过滤的文件类型（.JPG  .RAR .ZIP），以及他们相应的文件头特征
extern int sql_get_app_im_feature(char *SELECT_QUERY,MYSQL *sock);///YS 获取我们要审计的聊天应用相应的字段特征（飞信的其中一个字段的特征是FETION，具体查看IM_FEATURE表）
extern void arp_spoofind( );
MYSQL mysql;
extern char filepath[512];
struct pro_header_offset pro_header_offset_entity[32];///YS 这些变量仅用于一些特别的协议，像UDP里的TFTP，在传送文件时，前面的几个字节不是数据内容，所以会有一个OFFSET
pthread_mutex_t *mutex_for_cb;///YS 互斥缓冲cb_rawpacket用的
pthread_cond_t *notFull_cb, *notEmpty_cb;///YS 同步SOCKET写入线程，与缓冲读取线程

GHashTable *port_list;///YS KEY：协议名||VALUE：端口值（如：FTP  23)
GHashTable *udp_list;///YS  KEY：UDP—STREAM名||VALUE：时间戳+文件名（如：192.168.12.128023.13.22.1111_out     11123333ddsf.doc)
GHashTable *ip_list;///YS  KEY：用户IP ||VALUE：协议名（如：192.168.1.22  FTP)
GHashTable *file_name_ack_list;///YS  暂时没用
GHashTable *file_name_feature_list;///YS  KEY：协议名||VALUE：start="ddd"&len="aaa"（如：FTP  start=get&len=end),这个哈希表被用作查找协议
GHashTable *tcp_stream_list;///YS  KEY：TCP—STREAM名||VALUE：时间戳（如：192.168.12.128023.13.22.1111_out  112322213)，时间戳是用来算TCP流是否超时了
//GHashTable *tcp_stream_pool;
extern unsigned char arp_mac2[6];///ys 假网关mac

int debug_open ;///YS 调试是否打开
int http_enable;///YS HTTP访问是否打开
int search_enable;///YS 搜索审计是否打开
int bbs_enable;///YS BBS审计是否打开
int email_enable;///YS  邮件审计是否打开
char my_device [16];///YS 监听网卡
char  net_addr [8];///YS 子网
char arp_source[32]={0};///ys 假网关IP
char server_ip[32];///YS 服务器IP
extern char net_version[24];///ys VIPIP3
char match_file_type[support_file_num][32];///YS 要过滤的文件名的类型，.JPG  .RAR .ZIP等
char db_ip[24]={0};///ys 数据库服务器IP

extern int sock_client();///YS SOCKET通信线程
char ex_ip[24]={0};///ys VIPIP1，不会被拦截的IP，叫VIPIP
char ex_ip1[24]={0};///ys VIPIP2
char ex_ip2[24]={0};///ys VIPIP3
void iterator(gpointer key, gpointer value ,gpointer user_data)///YS 用于打印哈希表的值
{
    dbug("%s>>>>%p\n", (char*)key, value);
}



time_t timep;

struct packet_info my_packet;
//int rawpacket_putin_buff(u_char *arg, const struct pcap_pkthdr* pkthdr,const u_char * packet)
/*************************
该函数用于把接收到的数据包存入到环型缓冲区中
参数1，数据包的长度
参数2，数据包原始数据
**************************/
int rawpacket_putin_buff(const int len ,const u_char * packet)
{
    time(&timep);
    memset(&my_packet,0,sizeof(struct packet_info));
    my_packet.cap_size=len;///YS 数据包的长度
    my_packet.time= timep;///YS 数据包的拦截时间
//    dbug(">>>>>>>>>>>>  len %d\n",len);
    if(len>1518 || len <0) return 0;///YS 有的包是错，机会不大
    memcpy(my_packet.raw_packet,packet,len);
    pthread_mutex_lock(mutex_for_cb);
    if(cbIsFull(&cb_rawpacket))
    {
        pthread_cond_wait(notFull_cb, mutex_for_cb);
        cbWrite(&cb_rawpacket, &my_packet);///YS 写入到缓冲
    }
    else
    {
        cbWrite(&cb_rawpacket, &my_packet);///YS 写入到缓冲
    }
    pthread_cond_signal(notEmpty_cb);
    pthread_mutex_unlock(mutex_for_cb);
    return 0;
}


/*************************
该函数是从数据库中读取TCP应用里的固定端口的协议写入到哈希表中
参数1，SQL语句
参数2，MYSQL已连接的句柄
参数3，要插入的哈希表
参数4，是为了区分这两个函数
		sql_get_tcp_port_to_hash(SQL_GET_PORT,&mysql,port_list,0);//0000，这个是获取固定端口的协议
    sql_get_tcp_port_to_hash(GET_NAME_FEATURE,&mysql,file_name_feature_list,1);//11111，变相用于获取文件名的特征
**************************/
int sql_get_tcp_port_to_hash(char *SELECT_QUERY,MYSQL *sock,GHashTable * hash_table,int type)
{
    MYSQL_ROW row;
    MYSQL_RES *res;
    int i=0;
    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }

    while ((row = mysql_fetch_row(res)))
    {
        if(!type)///YS 0000，这个是获取固定端口的协议
        {
            if(strchr(row[1],'_'))snprintf(pro_header_offset_entity[i].pro_name,sizeof(pro_header_offset_entity[i].pro_name),"%s",row[1]+2);
            else snprintf(pro_header_offset_entity[i].pro_name,sizeof(pro_header_offset_entity[i].pro_name),"%s",row[1]);


            if(row[2]) pro_header_offset_entity[i].offset_type=atoi(row[2]);
            dbug("&&&&&&&&&&&&&   %s\n",row[3]);
            if(atoi(row[2])!=1)snprintf(pro_header_offset_entity[i].offset_feature,sizeof(pro_header_offset_entity[i].offset_feature),"%s",row[3]);
            else pro_header_offset_entity[i].offset_len=atoi(row[3]);
            g_hash_table_insert(hash_table, g_strdup(row[0]),g_strdup(row[1]));///这个hash表有过头了，以后换成数组
        }
        else///YS 1111，这个是获取固定端口的协议
        {

            char tmp[512];
            snprintf(tmp,sizeof(tmp),"start=%s#lenth=%s",row[1],row[2]);///YS 一开始是打算用START跟LENTH来确定TCP流中的文件名与长度，后来改变了，因为不是所有的协议都这样
            g_hash_table_insert(hash_table, g_strdup(row[0]),g_strdup(tmp));
        }
        i++;
    }
    mysql_free_result(res);
    return 0;
}



/*********************
系统配置函数
**********************/
int system_configure()
{
    debug_open=sql_get_configure_data(GET_CONFIG_DEBUG,&mysql,&debug_open,0,1);///YS 是否打开调试
    sql_get_configure_data(GET_CONFIG_SUB_NET,&mysql,net_addr,1,8);///YS 获取子网
    sql_get_configure_data(GET_CONFIG_NET_INTERFACE,&mysql,my_device,1,16);///YS 网卡设备
    sql_get_configure_data(GET_CONFIG_net_version,&mysql,net_version,1,24);


    sql_get_configure_data(GET_CONFIG_arp_source_ip,&mysql,arp_source,1,32);
    sql_get_configure_data(GET_CONFIG_db_ip,&mysql,db_ip,1,24);
    sql_get_configure_data(GET_CONFIG_ex_ip,&mysql,ex_ip,1,24);
    sql_get_configure_data(GET_CONFIG_ex_ip1,&mysql,ex_ip1,1,24);
    sql_get_configure_data(GET_CONFIG_ex_ip2,&mysql,ex_ip2,1,24);

    sql_get_configure_data(GET_CONFIG_SERVERIP,&mysql,server_ip,1,32);///YS 服务器IP，也是抓包机器
    sql_get_configure_data(GET_CONFIG_arp_mac,&mysql,NULL,arp_flag,32);

    http_enable=sql_get_configure_data(GET_CONFIG_http_enable,&mysql,&http_enable,0,1);///YS HTTP是否过滤
    search_enable=sql_get_configure_data(GET_CONFIG_search_enable,&mysql,&search_enable,0,1);///YS 搜索审计是否启用
    bbs_enable=sql_get_configure_data(GET_CONFIG_bbs_enable,&mysql,&bbs_enable,0,1);///YS BBS过滤功能
    email_enable=sql_get_configure_data(GET_CONFIG_email_enable,&mysql,&email_enable,0,1);///YS 邮件内容
    return 0;
}

struct search_info search[Audit_count];///YS 搜索特征字结构体
struct email_info email[Audit_count];///YS 邮件特征字结构体
struct bbs_info bbs[Audit_count];///YS BBS特征字结构体
struct email_html_info email_html[Audit_count];///YS EMAILREAD特征字结构体

/**************************
从数据库中读取数据，初始化我们的BBS结构体
***************************/
int  init_my_bbs_aduit_struct(MYSQL *sock)
{
    char SELECT_QUERY[1024];
    snprintf(SELECT_QUERY,sizeof(SELECT_QUERY),"select * from bbs_feature;");
    int j=0;

    MYSQL_ROW row;
    MYSQL_RES *res;

    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }
    //char name_temp[128];
    while ((row = mysql_fetch_row(res)))
    {
        if(row[1])
        {
            snprintf(bbs[j].host,64,"%s",row[1]);///YS BBS的HOST，如（163.BBS.COM等）
        }
        if(row[2])
        {
            bbs[j].case_value=atoi(row[2]);///YS 这个case_value是从1算起的，但是数组是从0算起，这就是为什么后面找到HOST后，返回的CASE_VALUE要1减了
        }
        if(row[3])
        {
            bbs[j].host_url_type=atoi(row[3]);///YS host_url_type的值决定了这个HOST是二级域名还是
        }
        if(row[4]&&row[3])
        {
            snprintf(bbs[j].host_2,64,"%s",row[4]);
        }
        if(row[5])
        {
            snprintf(bbs[j].content_feature_s,32,"%s",row[5]);///YS这个S是开始的意思start，内容这个字段的特征的开始，下同
        }
        if(row[6])
        {
            snprintf(bbs[j].content_feature_e,32,"%s",row[6]);///YS这个E是开始的意思END，内容这个字段的特征的结束，下同
        }
        if(row[7])
        {
            snprintf(bbs[j].post_url_feature_s,32,"%s",row[7]);
        }
        if(row[8])
        {
            if(!strcmp(row[8],"\\r\\n"))///YS 如果是\R\N的话，就换成实际的回车换行
                snprintf(bbs[j].post_url_feature_e,32,"%s","\x0D\x0A");
            else
            {
                snprintf(bbs[j].post_url_feature_e,32,"%s",row[8]);
            }
        }
        if(row[9])
        {
            snprintf(bbs[j].poster_s,32,"%s",row[9]);
        }
        if(row[10])
        {
            snprintf(bbs[j].poster_e,32,"%s",row[10]);
        }
        if(row[11])
        {
            snprintf(bbs[j].title_s,32,"%s",row[11]);
        }
        if(row[12])
        {
            snprintf(bbs[j].title_e,32,"%s",row[12]);
        }
        if(row[13])
        {
            snprintf(bbs[j].content_feature_s_2,32,"%s",row[13]);
        }
        if(row[14])
        {
            snprintf(bbs[j].content_feature_e_2,32,"%s",row[14]);
        }
        if(row[15])
        {
            snprintf(bbs[j].post_url_feature_s_2,32,"%s",row[15]);
        }
        if(row[16])
        {
            snprintf(bbs[j].post_url_feature_e_2,32,"%s",row[16]);
        }
        if(row[17])
        {
            snprintf(bbs[j].poster_s_2,32,"%s",row[17]);
        }
        if(row[18])
        {
            snprintf(bbs[j].poster_e_2,32,"%s",row[18]);
        }
        if(row[19])
        {
            snprintf(bbs[j].title_s_2,32,"%s",row[19]);
        }
        if(row[20])
        {
            snprintf(bbs[j].title_e_2,32,"%s",row[20]);
        }
        if(row[21])
        {
            snprintf(bbs[j].host_1,32,"%s",row[21]);
        }
        j++;
    }
    mysql_free_result(res);
    return 0;
}




/**************************
从数据库中读取数据，初始化我们的SEARCH结构体
***************************/
int  init_my_search_aduit_struct(MYSQL *sock)
{
    char SELECT_QUERY[1024];
    snprintf(SELECT_QUERY,sizeof(SELECT_QUERY),"select * from search_feature;");
    int j=0;

    MYSQL_ROW row;
    MYSQL_RES *res;

    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if ((!(res=mysql_store_result(sock))))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }
    while ((row = mysql_fetch_row(res)))
    {
        if(row[1])
        {
            snprintf(search[j].host,64,"%s",row[1]);
        }
        if(row[2])
        {
            search[j].case_value=atoi(row[2]);
        }
        if(row[3])
        {
            snprintf(search[j].key_word_s,32,"%s",row[3]);
        }
        if(row[4])
        {
            snprintf(search[j].key_word_e,32,"%s",row[4]);
        }
        if(row[5])
        {
            snprintf(search[j].key_word_s_2,32,"%s",row[5]);
        }
        if(row[6])
        {
            snprintf(search[j].key_word_e_2,32,"%s",row[6]);
        }
        if(row[7])
        {
            snprintf(search[j].key_word_s_3,32,"%s",row[7]);
        }
        if(row[8])
        {
            if(!strcmp(row[8],"\\r\\n"))
                snprintf(search[j].key_word_e,32,"%s","\x0D\x0A");
            else
            {
                snprintf(search[j].key_word_e_3,32,"%s",row[8]);
            }
        }
        if(row[9])
        {
            snprintf(search[j].auto_search,32,"%s",row[9]);
        }
        j++;
    }
    mysql_free_result(res);
    return 0;
}


/**************************
从数据库中读取数据，初始化我们的EMAIL  READ结构体
***************************/

int  init_email_html_aduit_struct(MYSQL *sock)
{
    char SELECT_QUERY[1024];
    snprintf(SELECT_QUERY,sizeof(SELECT_QUERY),"select * from email_read_feature;");
    int j=0;

    MYSQL_ROW row;
    MYSQL_RES *res;

    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if( (!(res=mysql_store_result(sock))))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        mysql_free_result(res);
        exit(1);
    }
    while ((row = mysql_fetch_row(res)))//ys ready for
    {
        if(row[1])
        {
            snprintf(email_html[j].host,64,"%s",row[1]);
        }
        if(row[2])
        {
            email_html[j].case_value=atoi(row[2]);
        }
        if(row[3])
        {
            snprintf(email_html[j].get_feature,244,"%s",row[3]);
        }
        if(row[4])
        {
            snprintf(email_html[j].http_respont_feature,244,"%s",row[4]);
        }
        if(row[5])
        {
            email_html[j].gzip=atoi(row[5]);
        }

        if(row[6])
        {
            snprintf(email_html[j].save_flag_1,244,"%s",row[6]);///YS 必需有这个字眼才可以保存
        }
        if(row[7])
        {
            snprintf(email_html[j].save_flag_2,244,"%s",row[7]);///YS 必需有这个字眼才可以保存
        }
        if(row[8])
        {
            email_html[j].pop3=atoi(row[8]);
        }



        if(row[9])
        {

            snprintf(email_html[j].content_s,244,"%s",row[9]);
        }
        if(row[10])
        {
            if(!strcmp(row[10],"\\r\\n.\\r\\n"))
                snprintf(email_html[j].content_e,244,"%s","\x0D\x0A\x0D\x0A\x2E\x0D\x0A");
            else
            {
                snprintf(email_html[j].content_e,244,"%s",row[10]);
            }
        }
        if(row[11])
        {
            dbug("HHHHHHHHHH%s \n",row[11]);
            snprintf(email_html[j].sender_s,244,"%s",row[11]);
        }
        if(row[12])
        {
            if(!strcmp(row[12],"\\r\\n"))
                snprintf(email_html[j].sender_e,244,"%s","\x0D\x0A");
            else
            {
                snprintf(email_html[j].sender_e,244,"%s",row[12]);
            }
        }
        if(row[13])
        {
            snprintf(email_html[j].receiver_s,244,"%s",row[13]);
        }
        if(row[14])
        {
            if(!strcmp(row[14],"\\r\\n"))
                snprintf(email_html[j].receiver_e,244,"%s","\x0D\x0A");
            else
            {
                snprintf(email_html[j].receiver_e,244,"%s",row[14]);
            }

        }
        if(row[15])
        {
            snprintf(email_html[j].title_s,244,"%s",row[15]);
        }
        if(row[16])
        {
            if(!strcmp(row[16],"\\r\\n"))
                snprintf(email_html[j].title_e,244,"%s","\x0D\x0A");
            else
            {
                snprintf(email_html[j].title_e,244,"%s",row[16]);
            }
        }
         if(row[17])
        {
            if(!strcmp(row[17],"\\r\\n"))
                snprintf(email_html[j].title_e2,244,"%s","\x0D\x0A");
            else
            {
                snprintf(email_html[j].title_e2,244,"%s",row[17]);
            }
        }

        j++;
    }
    mysql_free_result(res);
    return 0;
}


/**************************
从数据库中读取数据，初始化我们的EMAIL结构体，这一个是POSTEMAIL时用的
***************************/
int  init_my_email_aduit_struct(MYSQL *sock)
{
    char SELECT_QUERY[1024];
    snprintf(SELECT_QUERY,sizeof(SELECT_QUERY),"select * from email_feature;");
    int j=0;

    MYSQL_ROW row;
    MYSQL_RES *res;

    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        mysql_free_result(res);
        exit(1);
    }
    //char name_temp[128];
    while ((row = mysql_fetch_row(res)))
    {
        if(row[1])
        {
            snprintf(email[j].host,64,"%s",row[1]);
        }
        if(row[2])
        {
            email[j].case_value=atoi(row[2]);
        }
        if(row[3])
        {
            snprintf(email[j].content_feature_s,32,"%s",row[3]);
        }
        if(row[4])
        {
            snprintf(email[j].content_feature_e,32,"%s",row[4]);
        }
        if(row[5])
        {
            snprintf(email[j].sender_s,32,"%s",row[5]);
        }
        if(row[6])
        {
            if(!strcmp(row[6],"\\r\\n"))
                snprintf(email[j].sender_e,32,"%s","\x0D\x0A");
            else
            {
                snprintf(email[j].sender_e,32,"%s",row[6]);
            }

        }
        if(row[7])
        {
            snprintf(email[j].receiver_s,64,"%s",row[7]);
        }
        if(row[8])
        {
            if(!strcmp(row[8],"\\r\\n"))
                snprintf(email[j].receiver_e,64,"%s","\x0D\x0A");
            else
            {
                snprintf(email[j].receiver_e,64,"%s",row[8]);
            }

        }
        if(row[9])
        {
            snprintf(email[j].title_s,32,"%s",row[9]);
        }
        if(row[10])
        {
            if(!strcmp(row[10],"\\r\\n"))
                snprintf(email[j].title_e,32,"%s","\x0D\x0A");
            else
            {
                snprintf(email[j].title_e,32,"%s",row[10]);
            }

        }
        if(row[11])
        {
            snprintf(email[j].content_feature_s_2,32,"%s",row[11]);
        }
        if(row[12])
        {
            snprintf(email[j].content_feature_e_2,32,"%s",row[12]);
        }
        if(row[13])
        {
            snprintf(email[j].sender_s_2,32,"%s",row[13]);
        }
        if(row[14])
        {
            snprintf(email[j].sender_e_2,32,"%s",row[14]);
        }
        if(row[15])
        {
            snprintf(email[j].title_s_2,32,"%s",row[15]);
        }
        if(row[16])
        {
            snprintf(email[j].title_e_2,32,"%s",row[16]);
        }
        if(row[17])
        {
            snprintf(email[j].receiver_s_2,64,"%s",row[17]);
        }
        if(row[18])
        {
            snprintf(email[j].receiver_e_2,64,"%s",row[18]);
        }
        j++;
    }
    mysql_free_result(res);
    return 0;
}

/**************************
当我们的哈希表的条目和哈希表销毁时，会调该函数
***************************/
void free_data(gpointer hash_data)
{
    g_free(hash_data);
    hash_data=NULL;
}
extern char que_cache[];
int main(int argc, char **argv)
{

    get_config_local_file('a');
    srand((unsigned )time (NULL));///YS 随机数基数

    cache_count();///YS 定时任务函数，定时检查哪一些函数超时了
    int i=0;

    cbInit(&cb_rawpacket, cb_rawpacket_len);///YS 初始化我们的缓冲区
    sql_start(&mysql);///YS init sql
    ///YS 下面几个都是初始化我们的特征字结构体与系统配置
    init_my_email_aduit_struct(&mysql);
    init_my_search_aduit_struct(&mysql);
    init_my_bbs_aduit_struct(&mysql);
    init_email_html_aduit_struct(&mysql);
    system_configure();


    ///YS 信号量的初始化
    mutex_for_cb= (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex_for_cb, NULL);

    notFull_cb = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(notFull_cb, NULL);

    notEmpty_cb = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(notEmpty_cb, NULL);

    for(i=0; i<support_file_num; i++) memset(match_file_type[i],0,32);

    sql_get_file_name(GET_feature_list,&mysql);///YS 获取要过滤的文件类型（.JPG  .RAR .ZIP），以及他们相应的文件头特征
    sql_get_flow(GET_flow_list,&mysql);///YS 获取用户的流量
    sql_get_file_name_feature(GET_file_name_header_list,&mysql);///YS init pcre
    sql_get_app_im_feature(GET_app_im_feature_list,&mysql);///YS init pcre
    memset(filepath,0,sizeof(filepath));
    strcpy(filepath,argv[0]);
    ///YS 各哈希表的内容初始化
    port_list = g_hash_table_new_full(g_str_hash, g_str_equal,free_data,free_data);
    udp_list =  g_hash_table_new(g_str_hash, g_str_equal);
    file_name_ack_list= g_hash_table_new_full(g_str_hash, g_str_equal,free_data,free_data);
    ip_list=g_hash_table_new_full(g_str_hash, g_str_equal,free_data,free_data);///YS 放到IP表里面去  ，因后继的传送文件用的端口，都是动态的，所以用IP来做定断，像FTP ys//

    file_name_feature_list = g_hash_table_new_full(g_str_hash, g_str_equal,free_data,free_data);
    //tcp_stream_pool= g_hash_table_new(g_str_hash, g_direct_equal);
    tcp_stream_list = g_hash_table_new_full(g_str_hash, g_direct_hash,free_data,free_data);

    sql_get_tcp_port_to_hash(SQL_GET_PORT,&mysql,port_list,0);//0000
    sql_get_tcp_port_to_hash(GET_NAME_FEATURE,&mysql,file_name_feature_list,1);//11111



    ///YS 线程的创建
    pthread_t thread_rebuild_packet;
    if (pthread_create(&thread_rebuild_packet, NULL, (void*)get_syn_and_build_stream, NULL))
    {
        //dbug("Create thread_rebuild_packet error!\n");
        return -1;
    }
    else dbug("Create thread_rebuild_packet success\n");



    ///YS arp的创建
    pthread_t thread_arp_packet;
    if (pthread_create(&thread_arp_packet, NULL, (void*)arp_spoofind, NULL))
    {
        //dbug("Create thread_arp_packet error!\n");
        return -1;
    }
    else dbug("Create thread_arp_packet success\n");




    build_packet( );

    get_udp_prot();
    pthread_t psock_client;
    if (pthread_create(&psock_client, NULL, (void*)sock_client, NULL))
    {
        dbug("Create thread_rawpacket_pcap error!\n");
        return -1;
    }
    else
        dbug("Create psock_client success\n");
    post_error_host();

    ///YS 资源的等待与释放
    pthread_join(psock_client,NULL);
    pthread_join(thread_rebuild_packet,NULL);
    free(mutex_for_cb);
    free(notFull_cb);
    free(notEmpty_cb);
    cbFree(&cb_rawpacket);
    g_hash_table_destroy(port_list);
    g_hash_table_destroy(udp_list);
    g_hash_table_destroy(file_name_ack_list);
    g_hash_table_destroy(ip_list);
    g_hash_table_destroy(file_name_feature_list);
    g_hash_table_destroy(tcp_stream_list);

    mysql_close(&mysql);
    mysql_library_end();
    return  0;

}
