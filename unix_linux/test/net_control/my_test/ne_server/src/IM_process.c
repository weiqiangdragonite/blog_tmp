//聊天网络包的分析处理 现时只针对QQ作拦截
#include <libnet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <net/ethernet.h>
#include "packet_info.h"
#include "ringbuff.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

extern char sub_net[24];
extern GHashTable *qq_ip_list;
extern GHashTable *tcp_hash_white_list;
extern GHashTable *tcp_hash_person_list;
extern GHashTable *tcp_hash_group_list;
extern GHashTable *tcp_hash_whole_list;
extern GHashTable *user_hash_list;
extern int public_fun_put_Sip_into_cache(struct packet_info * packet_tmp);
extern GThreadPool *pool_tcp_rst;
extern void dbug(char *fmt,...);
extern GError *error;
extern CircularBuffer cb_http_b;  ///YS ringbuff b 用于保存待阻断http包
extern pthread_mutex_t *mutex_for_cb_http_b;///YS 用于同步http解析线程与阻断线程
extern time_t timep;
extern int tcp_packet_feature(struct packet_info * packet_tmp);

int tcp_start=0;
char qq_len_str[4]= {'\0'};
int qq_len=0;
/***************************
处理QQ的数据包，判断是否为QQ数据包
参数1“struct packet_info * packet_tmp”，是我们自定义的数据包
****************************/
int process_qq(struct packet_info * packet_tmp)
{
    struct tcphdr *tcp_header;// tcp header
    struct ip * ip_header; //ip header
    ///YS如果是03结尾的包，我们就算一下是不是QQ的，不是的话，直接返回
    if(packet_tmp->raw_packet[packet_tmp->cap_size-1]==0x03)
    {

        ip_header = (struct ip *)(packet_tmp->raw_packet + sizeof(struct ether_header));
        tcp_header= (struct tcphdr *)(packet_tmp->raw_packet+ sizeof(struct ether_header) + sizeof(struct ip));
        packet_tmp->packet_size=ntohs(ip_header->ip_len)-(tcp_header->th_off)*4 -(ip_header->ip_hl*4);
        tcp_start= packet_tmp->cap_size-packet_tmp->packet_size;
        if(packet_tmp->raw_packet[tcp_start+2]==0x02)
        {

            snprintf(qq_len_str,4,"%x%x",packet_tmp->raw_packet[tcp_start],packet_tmp->raw_packet[tcp_start+1]);
            sscanf(qq_len_str,"%x",&qq_len);
            if(qq_len==packet_tmp->packet_size)
            {
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }

    int g_push_i=1;
    char gid[8];

    //if(packet_tmp->raw_packet[packet_tmp->cap_size-1]==0x03)///YS QQ包，都是03结尾，不过这个信息的误报过高
    //if(memmem( packet_tmp->raw_packet,packet_tmp->cap_size,"\x00\x00\x00\x01\x01\x01\x00",7))
    strcpy(packet_tmp->behavior_type.detail,"qq");
    packet_tmp->behavior_type.already_process=3;
    char str_cmp[64]= {'\0'};
    char tcp_get_tmp[64]= {'\0'};
    if((!strstr(packet_tmp->dst_ip,sub_net)))///YS 是不是我们的SUB_NET
    {

        strcat(str_cmp,packet_tmp->src_ip);
        strcat(str_cmp,packet_tmp->dst_ip);///YS serverip+userip
        strcpy(tcp_get_tmp,packet_tmp->dst_ip);
    }
    else
    {

        strcat(str_cmp,packet_tmp->dst_ip);
        strcat(str_cmp,packet_tmp->src_ip);///YS serverip+userip
        strcpy(tcp_get_tmp,packet_tmp->src_ip);
    }
    // if(1)
    {
        if(g_hash_table_lookup(tcp_hash_white_list, str_cmp))
        {
            dbug("in while list !!!!!!");    ///YS 白名单,返回
            dbug(str_cmp);
            return 0;
        }
        else
        {
            time(&timep); ///YS 获得当前时间戳
            if(g_hash_table_lookup(tcp_hash_person_list, str_cmp))
            {
                dbug("in person list!!!! %s",str_cmp);///YS 在个人黑名单中
                g_hash_table_insert(tcp_hash_person_list, g_strdup(str_cmp), (gpointer)timep);///YS 更新缓存表的时间
                pthread_mutex_lock(mutex_for_cb_http_b);
                if(cbIsFull(&cb_http_b))
                {
                    dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                    pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                }
                else
                {
                    cbWrite(&cb_http_b, packet_tmp);///YS 写入拦截缓冲
                    g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);///YS 通知拦截处理线程池有数据包要拦截

                }
                pthread_mutex_unlock(mutex_for_cb_http_b);
                pthread_cond_signal(notEmpty_http_b);
                return 0;
            }
            else
            {
                memset(str_cmp,0,64);
                if(g_hash_table_lookup(user_hash_list,packet_tmp->src_ip))
                {
                    strcpy(gid,g_hash_table_lookup(user_hash_list,packet_tmp->src_ip));///YS 查找用户IP的组ID
                    strcpy(str_cmp,gid);
                    strcpy(packet_tmp->gid,gid);
                    snprintf(str_cmp,sizeof(str_cmp),"%s%s",gid,tcp_get_tmp);
                }
                // dbug("imp    >>>>>>>>>>>>     %s\n",str_cmp);
                if(g_hash_table_lookup(tcp_hash_group_list, str_cmp))///YS IP+GID
                {
                    dbug("in group list !!!!!! %d",timep);
                    g_hash_table_insert(tcp_hash_group_list, g_strdup(str_cmp), (gpointer)timep);///YS 更新缓存表时间
                    pthread_mutex_lock(mutex_for_cb_http_b);
                    if(cbIsFull(&cb_http_b))
                    {
                        dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                        pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                    }
                    else
                    {
                        cbWrite(&cb_http_b, packet_tmp);///YS 写入拦截缓冲
                        g_thread_pool_push(pool_tcp_rst,&g_push_i,&error);
                    }
                    pthread_mutex_unlock(mutex_for_cb_http_b);
                    pthread_cond_signal(notEmpty_http_b);
                    return 0;
                }
                else
                {
                    memset(str_cmp,0,64);
                    snprintf(str_cmp,sizeof(str_cmp),"%s",tcp_get_tmp);
                    if(g_hash_table_lookup(tcp_hash_whole_list, str_cmp))   ///YS 看一下是否在全局黑名单里
                    {
                        // dbug("whole_str >>>>>>>>>>%s",str_cmp);
                        g_hash_table_insert(tcp_hash_whole_list, g_strdup(str_cmp), (gpointer)timep);///YS 更新缓冲表里的时间
                        pthread_mutex_lock(mutex_for_cb_http_b);
                        if(cbIsFull(&cb_http_b))
                        {
                            dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                            pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                        }
                        else
                        {
                            cbWrite(&cb_http_b, packet_tmp);///YS 写入拦截缓冲
                            g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);

                        }
                        pthread_mutex_unlock(mutex_for_cb_http_b);
                        pthread_cond_signal(notEmpty_http_b);
                        return 0;
                    }
                }
            }
        }
        if(packet_tmp->cap_size>64) tcp_packet_feature(packet_tmp);///YS 如果这个包的有效长度》64，那就找一下这个包是否有特征字
    }

    return 0;
}
