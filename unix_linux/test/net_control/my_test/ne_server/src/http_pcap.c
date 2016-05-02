///ys HTTP 抓包相关函数
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
#include <pthread.h>
#include "public_function.h"
#include "config.h"
#include "packet_info.h"
#include "ringbuff.h"

extern CircularBuffer cb_http_a;  ///YS ringbuff a   用于保存捕获到HTTP的数据包
extern pthread_mutex_t *mutex_for_cb_http_a;///YS 用于http包捕获线程与http解析线程之间的互斥

extern void dbug(char *fmt,...);
extern pthread_mutex_t *mutex_for_sock;///YS用于http包捕获线程与tcp捕获线程之间互斥
extern pthread_cond_t *notFull_http_a, *notEmpty_http_a;///YS 用于同步http包捕获线程与http解析线程的信号量
extern char my_device[64];
extern GThreadPool *pool_http_process ;
extern GError *error;
extern int newsockfd;///YS 有效的TCP连接句柄
extern char ex_ip[24];
extern char ex_ip1[24];
extern char ex_ip2[24];
extern char dst_db_ip[32];

/************************
*把捕获的http数据包保存到ringbuff  a
*参数1“u_char *arg” 是回调函数传进来的用户自定义参数
*参数2“const struct pcap_pkthdr* pkthdr”， pkthdr是数据包的捕获时间与包的大小、捕获长度
*参数3“packet” ，是数据包的原始数据
************************/
void http_putin_buff(u_char *arg, const struct pcap_pkthdr* pkthdr,const u_char * packet) ///YS callback funtion
{
    int g_push_i=1;///YS g_push_i只作为参数填进去，因为填NULL会有问题
    struct packet_info my_packet;///YS 我们定义的数据包结构体
    //struct sock_buff my_sock_buff;///YS 我们定义的用SOCKET传数据包的结构体
    //memset(&my_sock_buff,0,sizeof(struct sock_buff));
    my_packet.cap_size=pkthdr->caplen; ///YS 抓到的长度

    pthread_mutex_lock(mutex_for_cb_http_a);
    if(cbIsFull(&cb_http_a))///YS 如果缓冲区满了
    {
        dbug("http_pcap>>>>>>>>>>here will fork the new thread to process the  packet");///YS 线程池里的线程不够用时，应该要新建线程，这个留以后优化
        pthread_cond_wait(notFull_http_a, mutex_for_cb_http_a);///YS 线程进入睡眠，并且释放锁，当有notFull_http_a信号出现时，才从睡眠状态醒过来
        memcpy(my_packet.raw_packet,packet,pkthdr->caplen);
        cbWrite(&cb_http_a, &my_packet);///YS 写入缓冲区
        g_thread_pool_push(pool_http_process, &g_push_i,&error);///YS 通知线程池，有数据来了

        ///YS 本来应该要把HTTP的包发去审计那边的，不过TCP拦截那边已经发包了，所以就不用了
    	/*my_sock_buff.len=pkthdr->caplen;
        memcpy(my_sock_buff.raw_packet,packet,pkthdr->caplen);
        if (newsockfd >0)
        {
            //if (write(newsockfd,(void*)&my_sock_buff,sizeof(struct sock_buff))< 0)
            if (write(newsockfd,(void*)&my_sock_buff,1544)< 0)
            {
                close(newsockfd);
                newsockfd=-1;
            }
        }*/
    }
    else
    {
        memcpy(my_packet.raw_packet,packet,pkthdr->caplen);
        cbWrite(&cb_http_a, &my_packet);
        g_thread_pool_push(pool_http_process, &g_push_i,&error);
     		///YS 本来应该要把HTTP的包发去审计那边的，不过TCP拦截那边已经发包了，所以就不用了
     		/* my_sock_buff.len=pkthdr->caplen;
        memcpy(my_sock_buff.raw_packet,packet,pkthdr->caplen);
        if (newsockfd >0)
        {

             n=write(newsockfd,(void*)&my_sock_buff,1544);
            if (n<1)
            {
                close(newsockfd);
                newsockfd=-1;
            }

           //  pthread_mutex_unlock(mutex_for_sock);
        }*/
    }
    pthread_mutex_unlock(mutex_for_cb_http_a);
    pthread_cond_signal(notEmpty_http_a);


    return;
}


/*************************
*捕获的http数据包线程
**************************/
void  http_pcap()
{
    int count =0;
    pcap_t *descr = NULL;
    struct bpf_program fp;
    char errbuf[1024];
    char http_bfp[2048];
    memset(http_bfp,sizeof(http_bfp),2048);
    ///YS HTTP拦截的条件
    snprintf(http_bfp,sizeof(http_bfp),"tcp port 80 and (not host %s) and (tcp[13] != 0x19) \
             and (not host %s ) and (not host %s ) and (not host %s )",dst_db_ip,ex_ip,ex_ip1,ex_ip2);
    memset(errbuf,0,1024);
    descr = pcap_open_live(my_device,MAXBYTES2CAPTURE,1,10,errbuf);///YS LIBPCAPAPI
		//if(public_fun_bpf_config(descr,"tcp port 80 and (not src host 192.168.1.222 or not dst host 192.168.1.222)",fp,0)==-1)//tcp port 80
    if(public_fun_bpf_config(descr,http_bfp,fp,0)==-1)///YS 配置我们的拦截条件
    {
        dbug("120\n");
//        exit(0);
    }

    pcap_loop(descr,-1,http_putin_buff,(u_char *)&count);//参数2，多少个包后就停止捕获，-1则没有限制
    pcap_close(descr);
}

