#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pcap/pcap.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <libnet.h>
#include "packet_info.h"
#include "ringbuff.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <glib.h>
#include "config.h"
#include <time.h>
#include <pthread.h>
extern GHashTable *tcp_hash_white_list;
extern GHashTable *tcp_hash_person_list;
extern GHashTable *tcp_hash_group_list;
extern GHashTable *tcp_hash_whole_list;
extern GHashTable *user_hash_list;
extern int public_fun_put_Sip_into_cache(struct packet_info * packet_tmp);
extern void dbug(char *fmt,...);
extern GHashTable *tcp_port_list;
extern int process_qq(struct packet_info * packet_tmp);
extern CircularBuffer cb_tcp_a;
extern pthread_mutex_t *mutex_for_cb_tcp_a;
extern pthread_cond_t *notFull_tcp_a, *notEmpty_tcp_a;

extern CircularBuffer cb_tcp_b;  ///YS ringbuff b 用于保存待阻断tcp包
extern pthread_mutex_t *mutex_for_cb_tcp_b;///YS 用于同步tcp解析线程与阻断线程
extern pthread_cond_t *notFull_tcp_b, *notEmpty_tcp_b;///YS 用于同步tcp解析线程与阻断线程
extern struct pcre_info s_pcre[pcre_num];

time_t timep;
extern int sx;
extern pthread_mutex_t *tcp_hash_mutex;
extern GThreadPool *pool_tcp_rst;
extern GError *error;
extern char sub_net[24];


/*************************
匹配TCP包的有效负载数据的特征字
参数1“struct packet_info * packet_tmp”，是我们自定义数据包
*************************/
int tcp_packet_feature(struct packet_info * packet_tmp)
{

    int i=0,rcCM=0;
    int ovector[OVECCOUNT]= {0};

    if(packet_tmp->behavior_type.already_process==3)
    {
        public_fun_put_Sip_into_cache(packet_tmp);
    }
    else
    {

        while(s_pcre[i].p_pcre)
        {
            ///YS 匹配pcre编译好的模式，成功返回正数，失败返回负数，54是直接跳过TCP的头，要需要有效数据
            rcCM = pcre_exec(s_pcre[i].p_pcre, NULL, (const char *)packet_tmp->raw_packet+54, packet_tmp->cap_size-54, 0, 0, ovector, OVECCOUNT);

            if (rcCM==PCRE_ERROR_NOMATCH )
            {
                // dbug(" No match ...%s",s_pcre[i].feature);
                i++;
                continue;
            }
            if(rcCM>=0)///YS 成功找到特征字
            {
                strcpy(packet_tmp->behavior_type.detail,s_pcre[i].name);
                //put_into_cache_server_ip(packet_tmp->dst_ip);
                public_fun_put_Sip_into_cache(packet_tmp); ///YS 找到特征字后，我们把该包的里的服务器IP填到我们的数据库中

                return 1;
            }

            i++;
        }
    }


    return 0;
}


/************************
*tcp数据包处理线程，处理过程如下，先取出数据包里的SRC_IP,DST_IP，然后果一下是否在白名单跟个人黑名单里，如果在的话就执行相应放行和拦截
*如果在黑白名单里没找到，就找相应的IP的用户组GID+IP，如果在组黑名单里找到，那就拦截，否则放行
*如果组黑名单里没找到，就找全局的，全局的只要IP就可以了
*如果都没找到，就看一下这个包的特征字，如果特正字找到了，就把服务器的IP保存下来，然后进行相应的拦截
*************************/

void process_tcp()
{
    struct tcphdr *tcp_header=NULL;
    struct ip * ip_header=NULL;
    struct packet_info packet_tmp;
    memset(&packet_tmp,0,sizeof(struct packet_info));
    int port_flag=1,g_push_i=1;

    char gid[8]= {0};
    char port[8]= {0};
    char str_cmp[tcp_process_strtmp_len]= {0};
    char tcp_get_tmp[tcp_process_strtmp_len]= {0};

    time(&timep);

    pthread_mutex_lock(mutex_for_cb_tcp_a);
    if(cbIsEmpty(&cb_tcp_a))///YS 如果是空的话，就等待，直到信号量notEmpty_http_a的产生
    {
        pthread_cond_wait(notEmpty_tcp_a,mutex_for_cb_tcp_a);
    }
    else
    {
        cbRead(&cb_tcp_a, &packet_tmp);///YS 从缓存中读出我们相应的数据，保存到packet temp
        pthread_mutex_unlock(mutex_for_cb_tcp_a);
        pthread_cond_signal(notFull_tcp_a);
    }





    ip_header = (struct ip *)(packet_tmp.raw_packet + sizeof(struct ether_header));
    strcpy(packet_tmp.src_ip,inet_ntoa(ip_header->ip_src));
    strcpy(packet_tmp.dst_ip,inet_ntoa(ip_header->ip_dst));

    process_qq(& packet_tmp);///YS 处理QQ，因为QQ有时会用443端口通信
    if(strstr(packet_tmp.dst_ip,sub_net))
    {
        return ;
    }


    tcp_header= (struct tcphdr *)(packet_tmp.raw_packet+ sizeof(struct ether_header) + sizeof(struct ip));
    packet_tmp.src_port=ntohs(tcp_header->th_sport);
    snprintf(port,sizeof(port),"%d",packet_tmp.src_port);

    if(g_hash_table_lookup(tcp_port_list,port))    ///YS 找一下这个数据包里的端口是不是在我们的端口哈希表里存在，例如，23TELNET这一类的
    {
        strcat(str_cmp,packet_tmp.dst_ip);///YS 找到端口的话，那就证明这个包是采用固定端口通信的，prot_flag=2
        strcat(str_cmp,port);///YS 这样的话要查哈希表的KEY，是IP+PROT
        port_flag=2;
    }
    else
    {
        strcat(str_cmp,packet_tmp.src_ip);
        strcat(str_cmp,packet_tmp.dst_ip);///YS 否则没有找到固定端口，要查哈希表的KEY是：serverip+useripys
        strcpy(tcp_get_tmp,packet_tmp.dst_ip);
    }


    if(g_hash_table_lookup(tcp_hash_white_list, str_cmp))
    {
        dbug("in while list !!!!!!");    ///YS 白名单IP+IP
        dbug(str_cmp);
        return;
    }
    else
    {
        if(g_hash_table_lookup(tcp_hash_person_list, str_cmp))
        {
            dbug("in person list!!!! %s",str_cmp);
            g_hash_table_insert(tcp_hash_person_list, g_strdup(str_cmp), (gpointer)timep);
            pthread_mutex_lock(mutex_for_cb_http_b);
            if(cbIsFull(&cb_http_b))
            {
                dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
            }
            else
            {
                cbWrite(&cb_http_b, &packet_tmp);///YS 在单名单里，把数据包写入拦截缓冲
                g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);///YS 通知拦截线程池

            }
            pthread_mutex_unlock(mutex_for_cb_http_b);
            pthread_cond_signal(notEmpty_http_b);
            return;
        }
        else
        {
            memset(str_cmp,0,tcp_process_strtmp_len);
            if(g_hash_table_lookup(user_hash_list,packet_tmp.src_ip))
            {
                strcpy(gid,g_hash_table_lookup(user_hash_list,packet_tmp.src_ip));
                strcpy(str_cmp,gid);
                strcpy(packet_tmp.gid,gid);
                if(port_flag==2)///YS 如果是固定端口拦截的，重新组合成新的KEY，再去查表
                {
                    strcat(str_cmp,port);
                }
                else
                {
                    snprintf(str_cmp,sizeof(str_cmp),"%s%s",gid,tcp_get_tmp);
                }
            }
            if(g_hash_table_lookup(tcp_hash_group_list, str_cmp))///YS str_cmp要么是IP+GID，要么是Port+GID（这是固定端口的）
            {

                dbug("in group list !!!!!! %d",timep);
                g_hash_table_insert(tcp_hash_group_list, g_strdup(str_cmp),(gpointer) timep);
                pthread_mutex_lock(mutex_for_cb_http_b);
                if(cbIsFull(&cb_http_b))
                {
                    dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                    pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                }
                else
                {
                    cbWrite(&cb_http_b, &packet_tmp);
                    g_thread_pool_push(pool_tcp_rst,&g_push_i,&error);

                }
                pthread_mutex_unlock(mutex_for_cb_http_b);
                pthread_cond_signal(notEmpty_http_b);
                return;
            }
            else
            {
                memset(str_cmp,0,tcp_process_strtmp_len);
                if(port_flag==2)///YS  同上，如果是固定端口的
                {
                    ///YS 直接使用PORT去查表
                    snprintf(str_cmp,sizeof(str_cmp),"%s",port);
                }
                else
                {
                    ///YS 直接使用IP去查表
                    snprintf(str_cmp,sizeof(str_cmp),"%s",tcp_get_tmp);
                }
                if(g_hash_table_lookup(tcp_hash_whole_list, str_cmp))///YS 如果找到了，则在全局黑名单里
                {
                    g_hash_table_insert(tcp_hash_whole_list, g_strdup(str_cmp),(gpointer) timep);
                    pthread_mutex_lock(mutex_for_cb_http_b);
                    if(cbIsFull(&cb_http_b))
                    {
                        dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                        pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                    }
                    else
                    {
                        cbWrite(&cb_http_b, &packet_tmp);///YS 写入到拦截缓冲中
                        g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);

                    }
                    pthread_mutex_unlock(mutex_for_cb_http_b);
                    pthread_cond_signal(notEmpty_http_b);
                    return;
                }
            }
        }
        if(sx==1)
        {
//            exit(0);
        }
        ///YS 如果都没有找到的话，而且这个包的长度》64，那就使用特征字进行查找
        if(packet_tmp.cap_size>64) tcp_packet_feature(&packet_tmp);
    }
}
