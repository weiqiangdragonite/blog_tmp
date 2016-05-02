///YS HTTP包处理线程，处理过程如下：
///YS 1、冲抓包缓冲区里读出数据包
///YS 2、判断包里是否有HTTP信息
///YS 3、有的话就取出HOST，和REFERER字段，取出的结果，像"www.baidu.com"，没有的话就放行
///YS 4、在取出URL后，再取出数据包里的IP，组合成IP+URL的形式
///YS 5、把这个IP+URL查一下哈希表，如果在白名单里跟个人黑名单里都没有找到的话就进行组黑名单查找
///YS 6、先到用户哈希表里查出IP对应的用户组如，1001，得出1001+URL
///YS 7、用这个新的值去查组黑名单，如果找到就写入拦截缓冲区，没找到的话就再找全局黑名单
///YS 8、直接拿URL进行全局黑名单查找，找到就写入拦截缓冲区
///YS 9、都没找到就放行该包
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
#include <pthread.h>
extern GHashTable *http_hash_white_list;
extern GHashTable *http_hash_person_list;
extern GHashTable *http_hash_group_list;
extern GHashTable *http_hash_whole_list;
extern GHashTable *http_hash_ALL_w_list;
extern GHashTable *http_hash_ALL_b_list;
extern GHashTable *user_hash_list;
extern void dbug(char *fmt,...);
extern CircularBuffer cb_http_a;
extern pthread_mutex_t *mutex_for_cb_http_a;
extern pthread_cond_t *notFull_http_a, *notEmpty_http_a;
extern int ex;
extern CircularBuffer cb_http_b;  ///YS ringbuff b 用于保存待阻断http包
extern pthread_mutex_t *mutex_for_cb_http_b;///YS 用于同步http解析线程与阻断线程
extern pthread_cond_t *notFull_http_b, *notEmpty_http_b;///YS 用于同步http解析线程与阻断线程
extern GThreadPool *pool_tcp_process ;
extern GThreadPool *pool_tcp_rst;
extern GError *error;
extern int process_qq(struct packet_info * packet_tmp); ///YS 是否是QQ协议


/***************************
*http数据包处理线程
****************************/
void process_http( )
{
    struct ip * ip_header;
    struct packet_info packet_tmp;

    char *p="0";
    char *p2="0";
    char gid[8];
    int len_end,g_push_i=1;
    char str_cmp[http_process_str_len];
    char http_get_tmp[http_process_str_len];
    memset(str_cmp,0,http_process_str_len);
    memset(http_get_tmp,0,http_process_str_len);

    ///YS 线程开始位置
    pthread_mutex_lock(mutex_for_cb_http_a);///YS 获得锁

    if(cbIsEmpty(&cb_http_a))///YS 如果是空的话，就等待，直到信号量notEmpty_http_a的产生
    {
        pthread_cond_wait(notEmpty_http_a,mutex_for_cb_http_a);
    }
    else
    {
        cbRead(&cb_http_a, &packet_tmp);///YS 从缓冲区里读出一个数据包，写入到packet temp里
        pthread_mutex_unlock(mutex_for_cb_http_a);
        pthread_cond_signal(notFull_http_a);
    }


    ip_header = (struct ip *)(packet_tmp.raw_packet + sizeof(struct ether_header));///YS ip头信息获取
    strcpy(packet_tmp.src_ip,inet_ntoa(ip_header->ip_src));///YS 获取IP_SRC
    strcpy(packet_tmp.dst_ip,inet_ntoa(ip_header->ip_dst));///YS 获取IP_DST

    process_qq(&packet_tmp);///YS 处理一下是否为QQ的包，因为QQ会用80端口

    if(g_hash_table_lookup(http_hash_ALL_w_list, packet_tmp.src_ip))
    {
        dbug("imp person  all white  list "); ///YS 查到在白名单（个人HTTP ALL，这个人可以无限制地使用80端口）
        return;
    }

    packet_tmp.packet_size=ntohs(ip_header->ip_len)-40;///YS 数据包的有效负载


    p=(char *)memmem( packet_tmp.raw_packet,packet_tmp.cap_size,"Host: ",6);///YS 查找是否有HOST这个字眼
    if(p)///YS 有的话就取出这个字眼后的内容，直接\R结束，内容通常为：www.baidu.com
    {
        p2=strchr(p,'\r');
        if(p2)
        {
            len_end=p2-p;
            if(len_end>sizeof(http_get_tmp))///YS 是否内容长度比BUFF的长度要长
            {
                snprintf(http_get_tmp,sizeof(http_get_tmp),"%s",p+6);
            }
            else
            {
                snprintf(http_get_tmp,len_end-5,"%s",p+6);
            }
        }
    }
    if(!http_get_tmp[0]) return ;///YS 如果内容为空的话就返回

    strcpy(str_cmp,http_get_tmp);
    strcat(str_cmp,packet_tmp.src_ip);///YS 获取IP，以进行黑名单个人查找
    packet_tmp.block_flag=http_flag;///YS 阻断的标志是HTTP，区分开HTTP，还是TCP
    if(g_hash_table_lookup(http_hash_white_list, str_cmp))
    {
        dbug("imp white  list %s",str_cmp); ///YS 白名单（个人），返回
        return ;
    }
    else
    {
        if(g_hash_table_lookup(http_hash_person_list, str_cmp))
        {
            dbug("imp person  list %s",str_cmp);///YS 黑名单（个人）

            pthread_mutex_lock(mutex_for_cb_http_b);
            if(cbIsFull(&cb_http_b))
            {
                dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
            }
            else
            {
                cbWrite(&cb_http_b, &packet_tmp);///YS 写入拦截缓冲区
                g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);///YS 通知线程池，有数据包要拦截
            }
            pthread_mutex_unlock(mutex_for_cb_http_b);
            pthread_cond_signal(notEmpty_http_b);
            return;
        }
        else
        {
            if(ex>0)
            {
//                exit(0);
            }
            memset(str_cmp,0,http_process_str_len);
            strcpy(str_cmp,http_get_tmp);///YS 如果个人的白黑名单里没有找到，则重新进行组黑名单查找字符串的匹配
            if(g_hash_table_lookup(user_hash_list,packet_tmp.src_ip))///YS 查找IP对应的用户组
                strcpy(gid,g_hash_table_lookup(user_hash_list,packet_tmp.src_ip));
            strcat(str_cmp,gid);///YS 连接字符串，如URL+1001
            if(g_hash_table_lookup(http_hash_group_list, str_cmp))///YS 进行组黑名单的查找
            {
                dbug("impgroup list %s",str_cmp);   ///YS 黑名单（组）
                pthread_mutex_lock(mutex_for_cb_http_b);
                if(cbIsFull(&cb_http_b))
                {
                    dbug("http_process  cb >>>>>>>>>>here will fork the new thread ");
                    pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                }
                else
                {
                    cbWrite(&cb_http_b, &packet_tmp);///YS 写入拦截缓冲区
                    g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);
                }
                pthread_mutex_unlock(mutex_for_cb_http_b);
                pthread_cond_signal(notEmpty_http_b);
                return;
            }
            else///YS 组黑名单没找到，找全局的
            {
                 //dbug("impTRRRYRTYTRYRY    %s",http_get_tmp); ///YS 黑名单（全局）
                if(g_hash_table_lookup(http_hash_whole_list, http_get_tmp))
                {
                    dbug("impwhole list %s",http_get_tmp); ///YS 黑名单（全局）
                    pthread_mutex_lock(mutex_for_cb_http_b);
                    if(cbIsFull(&cb_http_b))
                    {
                        dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");//for test
                        pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                    }
                    else
                    {
                        cbWrite(&cb_http_b, &packet_tmp);///YS 写入拦截缓冲区
                        g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);

                    }
                    pthread_mutex_unlock(mutex_for_cb_http_b);
                    pthread_cond_signal(notEmpty_http_b);
                    return;
                }
                else///YS 查找是否在http  all，是否全部都不能上
                {
                    if(g_hash_table_lookup(http_hash_ALL_b_list, packet_tmp.src_ip))///YS 该IP查出来，ALL BAN
                    {
                        dbug("imp person  all bla  list "); ///YS 黑名单ALL（个人）
                        pthread_mutex_lock(mutex_for_cb_http_b);
                        if(cbIsFull(&cb_http_b))
                        {
                            dbug("http_process  cb  >>>>>>>>>>here will fork the new thread  ");
                            pthread_cond_wait(notFull_http_b, mutex_for_cb_http_b);
                        }
                        else
                        {
                            cbWrite(&cb_http_b, &packet_tmp);///YS 写入拦截缓冲区
                            g_thread_pool_push(pool_tcp_rst, &g_push_i,&error);
                        }
                        pthread_mutex_unlock(mutex_for_cb_http_b);
                        pthread_cond_signal(notEmpty_http_b);
                        return;
                    }
                }
            }
        }
    }
}

