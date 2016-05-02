///YS TCP 抓包函数
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
#include "public_function.h"
#include "config.h"
#include "packet_info.h"
#include "ringbuff.h"
#include <pthread.h>

extern CircularBuffer cb_tcp_a;  ///YS ringbuff a   用于保存TCP捕获到（a） 数据包
extern pthread_mutex_t *mutex_for_cb_tcp_a; ///YS 用于同步  tcp包捕获（a）线程 与tcp解析线程
extern pthread_mutex_t *mutex_for_sock;
extern  void dbug(char *fmt,...);
extern pthread_cond_t *notFull_tcp_a, *notEmpty_tcp_a;///YS 用于同步  tcp包捕获（a）线程 与tcp解析线程
extern char my_device[64];
extern GThreadPool *pool_tcp_process ;
extern GError *error;
extern char ex_ip[24];
extern char ex_ip1[24];
extern char ex_ip2[24];
extern int newsockfd;
extern int ex;


extern char dst_db_ip[32];

extern MYSQL mysql;
extern char arp_source[32];///网关IP
/*
*把捕获的tcp数据包保存到ringbuff tcp a
*参数 arg 是回调函数传进来的
*pkthdr是数据包的捕获时间与包的大小、捕获长度
*packet是数据包的原始数据
*/
void tcp_putin_buff(u_char *arg, const struct pcap_pkthdr* pkthdr,const u_char * packet)
{
    int g_push_i=1;
    struct packet_info my_packet_tcp_pcap;
    memset(&my_packet_tcp_pcap,0,sizeof(struct packet_info));
    my_packet_tcp_pcap.cap_size=pkthdr->caplen;

    pthread_mutex_lock(mutex_for_cb_tcp_a);
    if(cbIsFull(&cb_tcp_a))
    {
        printf("tcp_cap>>>>>>>>>here will fork the new thread to process the  packet  ");
        pthread_cond_wait(notFull_tcp_a, mutex_for_cb_tcp_a);
        memcpy(my_packet_tcp_pcap.raw_packet,packet,pkthdr->caplen);
        cbWrite(&cb_tcp_a, &my_packet_tcp_pcap);///YS 写入缓冲区
        g_thread_pool_push(pool_tcp_process, &g_push_i,&error);
    }
    else
    {
        memcpy(my_packet_tcp_pcap.raw_packet,packet,pkthdr->caplen);
        cbWrite(&cb_tcp_a, &my_packet_tcp_pcap);///YS 写入缓冲区
        g_thread_pool_push(pool_tcp_process, &g_push_i,&error);


        struct sock_buff my_sock_buff;
        memset(&my_sock_buff,0,sizeof(struct sock_buff));
        my_sock_buff.len=pkthdr->caplen;
        memcpy(my_sock_buff.raw_packet,packet,pkthdr->caplen);
        if (newsockfd >0)///YS 如果审计端连接上了，就把数据向外发
        {
            if (write(newsockfd,(void*)&my_sock_buff,1544)< 0)///YS 发到审计端的数据
            {
                close(newsockfd);
                newsockfd=-1;
            }
        }
    }
    pthread_mutex_unlock(mutex_for_cb_tcp_a);
    pthread_cond_signal(notEmpty_tcp_a);
    return;
}

/*
*捕获的tcp数据包线程
*/
void  tcp_pcap()
{
    pcap_t *descr = NULL;
    struct bpf_program fp;
    char tcp_bpf[2048]={0};
    memset(tcp_bpf,0,sizeof(tcp_bpf));
    char errbuf[1024]={0};
    memset(errbuf,0,1024);
    snprintf(tcp_bpf,sizeof(tcp_bpf),"tcp and\
                             (not host %s )\
                             and (not port 6788) and (not port 6789) and (tcp[13] != 0x4) and (tcp[13] != 0x19)  \
             and (tcp[13] != 0x14) and (not host %s)and (not host %s)\
             and (not host %s)  and (not host %s)",dst_db_ip,ex_ip,arp_source,ex_ip1,ex_ip2);///YS 我们的过虑条件
    descr = pcap_open_live(my_device,MAXBYTES2CAPTURE,1,10,errbuf);
    if(public_fun_bpf_config(descr,tcp_bpf,fp,0)==-1)
    {
        dbug("bpf config failed!\n");
//        exit(0);
    }
    else
    {
        dbug("create tcp pcap thread success\r");
    }
    pcap_loop(descr,-1,tcp_putin_buff,NULL);///YS 参数2，多少个包后就停止捕获
    pcap_close(descr);
}

