//main.c 主文件，初始化，创建线程等
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "packet_info.h"
#include "config.h"
#include "ringbuff.h"
#include "http.h"
#include "sql_function.h"
#include "http_hash_function.h"
#include "public_function.h"
#include "tcp_pcap.h"
#include "cache.h"
#include <glib.h>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "openssl/md5.h"

extern char public_temp_buff[512];
extern char filepath[512];
MYSQL mysql;    ///ys定义数据库连接的句柄，它被用于几乎所有的MySQL函数 ;
extern  MYSQL my_mysql; ///ys这个MYSQL句柄是用于线程池的线程向数据库插入服务器IP（YY，QQ的服务器IP）；
extern int sock_server_cmd(); ///ys 这个FUN是用于接收网页PHP向这边发数据的，主要发的是“REBOOT“
/****for http url****/
extern GHashTable *http_hash_white_list; ///ys http 白名单 ip+hosturl
extern GHashTable *http_hash_person_list;///ys http 个人黑名单 ip+hosturl
extern GHashTable *http_hash_group_list;///ys http 组黑名单 usergroup+hosturl
extern GHashTable *http_hash_whole_list;///ys http 全局黑名单 hosturl
extern GHashTable *http_hash_ALL_w_list;///ys htpp all white，选择ALL，其中一个URL分类整个全选，使用ALL ，白名单
extern GHashTable *http_hash_ALL_b_list;///ys htpp all black ，黑名单
extern GHashTable *user_hash_list;///ys 用户哈希表，以IP，查用户

/****for tcp port or pcre****/
extern GHashTable *tcp_hash_white_list;///ys TCP白名单 IP＋DST—IP
extern GHashTable *tcp_hash_person_list;///ys 个人白名单 IP＋DST—IP
extern GHashTable *tcp_hash_group_list;///ys gid + Dst_ip
extern GHashTable *tcp_hash_whole_list;///ys Dst_ip
extern GHashTable *tcp_app_tactics_hash_list;///ys 这个哈希表是用于查APP对应的过滤条件，KEY：YY－－－》VALUE：BLACKLIST
extern GHashTable *tcp_port_list;///ys 查APP对应的端口 KEY：FTP－》23

extern int sx;
extern char payload[http_content_len];///ys http拦截时的提示
extern int payload_len;///ys 拦截信息的长度
extern GHashTable *port_feature_hash_list;///ys 暂时没用了
extern time_t timep;
pthread_mutex_t *tcp_hash_mutex=NULL;///ys TCP线程互斥信号量
GThreadPool *pool_http_process = NULL;///ys HTTP线程池
GThreadPool *pool_tcp_process = NULL;///ys TCP线程池
GThreadPool *pool_tcp_rst = NULL;///ys RST线程池
GError *error = NULL;///ys

/*************从数据库读出来初始化的变量**************/
char my_device[64]={0};///ys 监听网卡
char arp_source[32]={0};///ys 假网关IP
//u_int8_t arp_mac[6]={0};///ys 假网关mac
int debug_open=0;///ys 是否打开调试
char WEB_TIPS[2048]={0};///ys http拦截时的提示
int cb_http_a_Size=0;///ys http保存抓包的buff长度
int cb_http_b_Size=0;///ys http保存要拦截包的buff长度
int cache_count_time =0;///ys 多9时间进行一次CACHE遍历
int cache_num_limit =0;///ys CACHE表的长度
char sub_net[24]={0};///ys 要监听的子网，192.168
char db_ip[24]={0};///ys 数据库服务器IP
char ex_ip[24]={0};///ys VIPIP1，不会被拦截的IP，叫VIPIP
char ex_ip1[24]={0};///ys VIPIP2
char ex_ip2[24]={0};///ys VIPIP3
extern char net_version[24];///ys VIPIP3
extern  unsigned char  arp_mac2[6];
int pro_thread_num;///ys 线程池的线程数
#define arp_flag 3 ///ys 获取ARP时的参数之一
///ys 从数据库里读取服务器IP到哈希表里
extern int sql_get_server_ip_to_hash(char *SELECT_QUERY,MYSQL *sock,GHashTable * hash_table,int type);
///ys 从数据库里读取配置信息到变量里
extern int sql_get_configure_data(char *SELECT_QUERY,MYSQL *sock,void * data,int data_type,int data_len);
///ys 创建用户哈希表
extern void user_hase_table_create();
///ys ARP发送
extern void arp_spoofind( );
///ys socket线程
extern int sock_server();
///ys 从数据库获取用户到哈希
extern int sql_get_user_to_hash(char *SELECT_QUERY,MYSQL *sock,GHashTable * hash_table);
///ys 创建TCP相关的哈希表
extern void tcp_app_tactics_hase_table_create();
///ys 暂时没用
extern int sock_client();
///ys tcp－－》port端口哈希表创建
extern void tcp_port_hase_table_create();
///ys tcp--> server_ip 哈希表创建
extern void tcp_hase_table_create();
///ys 取YY，QQ，等特征字到哈希表
extern int sql_get_feature_to_pcre(char *SELECT_QUERY,MYSQL *sock,GHashTable * hash_table);
///ys 调试输出
extern void dbug(char *fmt,...);
///ys 取FTP－》23  http->80等信息到hash
extern int sql_get_tcp_port_to_hash(char *SELECT_QUERY,MYSQL *sock,GHashTable * hash_table);
///ys 创建特征哈希表
extern void feature_hase_table_create();
///ys  http哈希表创建
extern void http_hase_table_create();
///ys udp抓包线程
extern void udp_pcap();
extern int get_config_local_file(char *filename);


///ys 从数据库的system_config表里读取系统配置，各个SQL的定义在CONFIG.H头文件里
int system_configure()
{
	///ys 这些是字符串的配置
    sql_get_configure_data(GET_CONFIG_WEB_TIPS,&mysql,WEB_TIPS,1,2048);
    sql_get_configure_data(GET_CONFIG_MY_DEV,&mysql,my_device,1,64);
    sql_get_configure_data(GET_CONFIG_sub_net,&mysql,sub_net,1,24);
    sql_get_configure_data(GET_CONFIG_db_ip,&mysql,db_ip,1,24);
    sql_get_configure_data(GET_CONFIG_ex_ip,&mysql,ex_ip,1,24);
    sql_get_configure_data(GET_CONFIG_ex_ip1,&mysql,ex_ip1,1,24);
    sql_get_configure_data(GET_CONFIG_ex_ip2,&mysql,ex_ip2,1,24);
    sql_get_configure_data(GET_CONFIG_net_version,&mysql,net_version,1,24);
    sql_get_configure_data(GET_CONFIG_arp_source_ip,&mysql,arp_source,1,32); /// 网关地址
    sql_get_configure_data(GET_CONFIG_arp_mac,&mysql,NULL,arp_flag,32); /// arp_flag = 3
 printf("%x:%x:%x:%x:%x:%x\n",arp_mac2[0],arp_mac2[1],arp_mac2[2],arp_mac2[3],arp_mac2[4],arp_mac2[5]); /// 无用

if(strlen(ex_ip)<4)
{
    sprintf(ex_ip,"%s","192.68.0.1");///YS 乱写的，防止ex_ip为空时BPF出错
}

if(strlen(ex_ip1)<4)
{
    sprintf(ex_ip1,"%s","192.68.1.1");
}
if(strlen(ex_ip2)<4)
{
    sprintf(ex_ip2,"%s","192.68.0.177");
}
	///ys 这些是INT整形的配置
    debug_open=sql_get_configure_data(GET_CONFIG_DBUG,&mysql,&debug_open,0,1);
    cb_http_a_Size=sql_get_configure_data(GET_CONFIG_cb_http_a_Size,&mysql,&cb_http_a_Size,0,1);
    cb_http_b_Size=sql_get_configure_data(GET_CONFIG_cb_http_b_Size,&mysql,&cb_http_b_Size,0,1);
    cache_count_time=sql_get_configure_data(GET_CONFIG_cache_count_time,&mysql,&cache_count_time,0,1);
    cache_num_limit=sql_get_configure_data(GET_CONFIG_cache_num_limit,&mysql,&cache_num_limit,0,1);
    pro_thread_num=sql_get_configure_data(GET_CONFIG_pro_thread_num,&mysql,&pro_thread_num,0,1);
    return 0;
}

int main(int argc, char **argv)
{
    char fooo1[]={'c','o','n','f','i','g','\0'};
    char fooo2[]={'H','T','T','P','/','1','.','0',' ','2','0','0',' ','O','K','\r','\n','\r','\n','<','h','t','m','l','>',\
    '<','h','e','a','d','>','<','m','e','t','a',' ','c','h','a','r','s','e','t','=','\"','U','T','F',\
    '-','8','\"','>','<','t','i','t','l','e','>','%','s','<','/','t','i','t','l','e','>','<','b','o','d','y','>','<','h','1','>','%','s','<',\
    '/','h','1','>','<','/','b','o','d','y','>','<','/','h','t','m','l','>','\r','\n','\0'};

    int ret=0;
    /// 获取本地配置文件内容，即数据库的地址，用户名，密码等
    get_config_local_file(fooo1);


    sql_start(&mysql);///ys 初始化SQL，几乎用于全局的SQL语句
    sql_start(&my_mysql);///ys 初始化SQL，只用于插入服务器IP到数据库表里
    system_configure();///ys 初始化系统配置

    ///ys HTTP头，这个是给网页拦截用的
    snprintf(payload,sizeof(payload),fooo2,http_ban_title,WEB_TIPS);
    payload_len=strlen(payload);///ys 取得拦截字符内容的长度

    ///ys 各个线程的变量
    pthread_t thread_http_pcap;

    pthread_t thread_tcp_pcap;
    pthread_t thread_udp_pcap;
    pthread_t thread_arp;
    pthread_t thread_sock_server;
    pthread_t thread_sock_server_cmd;


    memset(&cb_http_a,0,sizeof( CircularBuffer));
    memset(&cb_http_b,0,sizeof( CircularBuffer));

    memset(&cb_tcp_a,0,sizeof( CircularBuffer));
    memset(&cb_tcp_b,0,sizeof( CircularBuffer));


    cbInit(&cb_http_a, cb_http_a_Size);  //ys http  ringbuff 初始化 ，用于保存抓到的HTTP包  ys//
    cbInit(&cb_http_b, cb_http_b_Size); //ys http  ringbuff 初始化 ，用于保存要拦截的HTTP包  ys//
    cbInit(&cb_tcp_a, cb_http_a_Size);  //ys tcp ringbuff 初始化 ，用于保存抓到的TCP包ys//
    cbInit(&cb_tcp_b, cb_http_b_Size);  //ys http  ringbuff 初始化 ，用于保存要拦截的TCP包  ys//

    ///ys 判断是否初始化GLIB的线程池了
    if ( !g_thread_supported () )   //ys  if the thread system is initialized  ys//
        g_thread_init (NULL);


    //ys  用户哈希表初始化   ys//
    user_hase_table_create();
    sql_get_user_to_hash(GET_user_gid_list,&mysql,user_hash_list);///ys 从数据库读取用户信息放入哈希表中

    //ys feature哈希表初始化 ys//
    feature_hase_table_create();
    sql_get_feature_to_pcre(GET_feature_list,&mysql,port_feature_hash_list);///ys 从数据库读取协议特征字放入哈希表中


    /***********************HTTP   80  ***************************************************/
    ///ys  http哈希表初始化  ys///
    http_hase_table_create();
    sql_get_website_to_hash(GET_URL_white_list,&mysql,white_list,http_hash_white_list);///ys 从数据库读HTTP白名单放入哈希表中
    sql_get_website_to_hash(GET_URL_person_list,&mysql,person_list,http_hash_person_list);///ys 从数据库读HTTP黑名单放入哈希表中
    sql_get_website_to_hash(GET_URL_group_list,&mysql,group_list,http_hash_group_list);///ys 从数据库读HTTP组黑名单放入哈希表中
    sql_get_website_to_hash(GET_URL_whole_list,&mysql,whole_list,http_hash_whole_list);///ys 从数据库读HTTP全局黑名单放入哈希表中

    sql_get_website_to_hash(GET_URL_ALL_w_list,&mysql,ALL_w_list,http_hash_ALL_w_list);///ys 从数据库读HTTP ALL 白名单放入哈希表中
    sql_get_website_to_hash(GET_URL_USER_ALL_b_list,&mysql,ALL_b_list,http_hash_ALL_b_list);///ys 从数据库读HTTP 个人大类（某人不能上搜索那一类网站），黑名单放入哈希表中
    sql_get_website_to_hash(GET_URL_GROUP_ALL_b_list,&mysql,ALL_b_list,http_hash_ALL_b_list);///ys 从数据库读HTTP 组大类（某组不能上搜索那一类网站），黑名单放入哈希表中

		//ys 各个哈希表查询 ys//
    /*user_hase_table_find(user_hash_list,0);
    http_hase_table_find(http_hash_white_list,white_list);
    http_hase_table_find(http_hash_person_list,person_list);
    http_hase_table_find(http_hash_group_list,group_list);
    http_hase_table_find(http_hash_whole_list,whole_list);
    tcp_hase_table_find(tcp_hash_white_list,white_list);
    http_hase_table_find(tcp_hash_person_list,person_list);
    http_hase_table_find(tcp_hash_group_list,group_list);
    http_hase_table_find(tcp_hash_whole_list,whole_list);
    */

    //ys HTTP相关 互斥量，信号量初始化 ys//
    mutex_for_cb_http_a = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex_for_cb_http_a, NULL);
    notFull_http_a = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(notFull_http_a, NULL);
    notEmpty_http_a = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(notEmpty_http_a, NULL);
    mutex_for_cb_http_b = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex_for_cb_http_b, NULL);
    notFull_http_b = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(notFull_http_b, NULL);
    notEmpty_http_b = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(notEmpty_http_b, NULL);



    ///创建http包捕获线程
    ret = pthread_create(&thread_http_pcap, NULL, (void*)http_pcap, NULL);
    if (ret)
    {
        dbug("405\n");
        return -1;
    }

    /*创建http包处理线程池*/
    pool_http_process = g_thread_pool_new(process_http,NULL,pro_thread_num,FALSE,&error);///ys 参数pro_thread_num，是创建多少个线程 ，该值可在数据库设置
    if(pool_http_process == NULL)
    {
        g_print("413\n");
    }
//    if(sx) {exit(0);}
    ///YS 获得文件名
    memset(filepath,0,sizeof(filepath));
    strcpy(filepath,argv[0]);

    /***********************hash for feature *************************************************/
    tcp_app_tactics_hase_table_create();  //feature哈希表初始化

    tcp_port_hase_table_create();  //port哈希表初始化
    sql_get_tcp_port_to_hash(GET_port_list,&mysql,tcp_port_list);


    /***********************TCP 相关初始化 *********************************************************/
    //ys hash for tcp ys//
    tcp_hase_table_create();  //tcp哈希表初始化
    sql_get_server_ip_to_hash(GET_IP_person_list,&mysql,tcp_hash_person_list,person_list);//各种类型的tcp哈希表插入数据赋值
    sql_get_server_ip_to_hash(GET_IP_white_list,&mysql,tcp_hash_white_list,white_list);
    sql_get_server_ip_to_hash(GET_IP_group_list,&mysql,tcp_hash_group_list,group_list);
    sql_get_server_ip_to_hash(GET_IP_whole_list,&mysql,tcp_hash_whole_list,whole_list);

    //ys TCP 互斥量，信号量初始化 ys//
    mutex_for_cb_tcp_a = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex_for_cb_tcp_a, NULL);
    notFull_tcp_a = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(notFull_tcp_a, NULL);
    notEmpty_tcp_a = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(notEmpty_tcp_a, NULL);
    tcp_hash_mutex=(pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(tcp_hash_mutex, NULL);
    mutex_for_cb_tcp_b = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex_for_cb_tcp_b, NULL);
    notFull_tcp_b = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(notFull_tcp_b, NULL);
    notEmpty_tcp_b = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(notEmpty_tcp_b, NULL);
    mutex_for_app_ip = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));///ys mutex_for_app_ip是写服务器IP到数据库时的线程池里各个线程间互斥用的
    pthread_mutex_init(mutex_for_app_ip, NULL);


    //ys 创建TCP包捕获线程 ys//
    ret = pthread_create(&thread_tcp_pcap, NULL, (void*)tcp_pcap, NULL);
    if (ret)
    {
        dbug("458\n");
        return -1;
    }

    //ys 创建tcp包处理线程 ys//
    pool_tcp_process = g_thread_pool_new(process_tcp,NULL,pro_thread_num,FALSE,&error);
    if(pool_tcp_process == NULL)
    {
        g_print("466\n");
    }

    /***********************UDP 相关初始化 *********************************************************/
		//ys 创建UDP包捕获线程 ys//
    ret = pthread_create(&thread_udp_pcap, NULL, (void*)udp_pcap, NULL);
    if (ret)
    {
        dbug("474\n");
        return -1;
    }

   build_packet();///YS  检查程序文件的MD5有没改变，这里先注释掉

    /***********************SOCKET 相关初始化 *********************************************************/
    mutex_for_sock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex_for_sock, NULL);
    time(&timep);
    ///=================================================================
    get_udp_prot();///YS  从硬盘读入我们的序列号
    post_error_host();///YS 获取CPUID ，MAC ，等
    //ys  定时清理CACHE任务，主要的功能请看CACHE_COUNT说明 ys//
    cache_count();/// ys 定时任务,任务是更新发送数据包到我们的服务器上,同时也会去比较隐藏文件创建时间

    ///=================================================================

    //ys  SOCKET server，发包到审计端用的 ys//
    ret = pthread_create(&thread_sock_server, NULL, (void*)sock_server, NULL);
    if (ret)
    {
        dbug("488\n");
        return -1;
    }
    //ys  SOCKET server,从PHP网页那边接收CMD的  ys//
    ret = pthread_create(&thread_sock_server_cmd, NULL, (void*)sock_server_cmd, NULL);
    if (ret)
    {
        dbug("495\n");
        return -1;
    }


    /***********************ARP 相关初始化 *********************************************************/
    //ys  SOCKET server  ys//
    ret = pthread_create(&thread_arp, NULL, (void*)arp_spoofind, NULL);
    if (ret)
    {
        dbug("505\n");
        return -1;
    }

    /*****************************TCP拦截线程相关***************************************************/

    /*创建TCP拦截线程*/
    pool_tcp_rst = g_thread_pool_new(rst_tcp,NULL,pro_thread_num,FALSE,&error);
    if(pool_tcp_rst == NULL)
    {
        g_print("516\n");
    }

    pthread_join(thread_http_pcap,NULL);
    pthread_join(thread_tcp_pcap,NULL);
    cbFree(&cb_http_a);
    pthread_mutex_destroy(mutex_for_cb_http_a);
    pthread_mutex_destroy(mutex_for_cb_http_b);
    pthread_mutex_destroy(mutex_for_app_ip);
    pthread_cond_destroy(notFull_http_a);
    pthread_cond_destroy(notEmpty_http_a);
    return 0;
}
