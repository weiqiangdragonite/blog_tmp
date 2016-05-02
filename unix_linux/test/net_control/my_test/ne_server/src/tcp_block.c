///YS TCP阻断函数
#include <libnet.h>
#include "config.h"
#include "packet_info.h"
#include "ringbuff.h"
#include <pthread.h>


extern CircularBuffer cb_http_b;
extern CircularBuffer cb_tcp_b;
extern pthread_mutex_t *mutex_for_cb_http_b;
extern int public_fun_get_packet_info(const u_char * packet,struct packet_info  *my_packet);
extern pthread_cond_t *notFull_http_b, *notEmpty_http_b;
extern char my_device[64];
//int rst_flag =1;
char errbuf[LIBNET_ERRBUF_SIZE];
char payload[http_content_len];
int payload_len;

/***********************************
*  TCP阻断函数，主要调用了LIBNET这个库来构造RST包
*  参数 my_packet是要阻断的包
*
***********************************/

int rst_tcp3(struct packet_info *my_packet)
{


    libnet_t *l;
    u_long src_ip, dst_ip;
    char errbuf[LIBNET_ERRBUF_SIZE];

    l = libnet_init(LIBNET_LINK,"re0",errbuf);
    src_ip = libnet_name2addr4(l,my_packet->src_ip,LIBNET_DONT_RESOLVE);///YS 先转换IP
    dst_ip = libnet_name2addr4(l,my_packet->dst_ip,LIBNET_DONT_RESOLVE);

    if(my_packet->block_flag==http_flag)///YS ，看一下是不是HTTP包要拦截的，如果是的话，不仅要拦截，还要发一个HTTP包
    {

        //2 TCP  send packet to SRC 192.168.1.22

        libnet_build_tcp(my_packet->dst_port, my_packet->src_port,   my_packet->ack,  my_packet->seq+my_packet->packet_size,   TH_URG|TH_PUSH | TH_ACK | TH_FIN,  my_packet->win,  0,0, LIBNET_TCP_H+payload_len  ,(u_int8_t *)payload, payload_len,l,0);
        libnet_build_ipv4(LIBNET_IPV4_H+LIBNET_TCP_H+payload_len,0,242,0,64,IPPROTO_TCP,0,dst_ip,src_ip,NULL,0,l,0);
        libnet_build_ethernet(my_packet->src_mac,my_packet->dst_mac,ETHERTYPE_IP,NULL,0,l,0);
        libnet_write(l);///YS 发送数据包
        //libnet_write(l);


				//3 TCP  RST send packet to SRC
        libnet_build_tcp(my_packet->dst_port,my_packet->src_port,my_packet->ack,0,TH_RST,0,0,0,LIBNET_TCP_H,NULL,0,l,0);
        libnet_build_ipv4(LIBNET_IPV4_H+LIBNET_TCP_H+0,0,242,0,64,IPPROTO_TCP,0,dst_ip,src_ip,NULL,0,l,0);
        libnet_build_ethernet(my_packet->src_mac,my_packet->dst_mac,ETHERTYPE_IP,NULL,0,l,0);
        libnet_write(l);
        //1  TCP  RST to DST


        libnet_build_tcp(my_packet->src_port,my_packet->dst_port,my_packet->seq+my_packet->packet_size,0,TH_RST,my_packet->win,0,0,LIBNET_TCP_H,NULL,0,l,0);
        libnet_build_ipv4(LIBNET_IPV4_H+LIBNET_TCP_H+0,0,242,0,64,IPPROTO_TCP,0,src_ip,dst_ip,NULL,0,l,0);
        libnet_build_ethernet(my_packet->dst_mac,my_packet->src_mac,ETHERTYPE_IP,NULL,0,l,0);
        libnet_write(l);



    }
    else///YS 这里是拦截TCP应用的，非HTTP的
    {

        libnet_build_tcp(my_packet->dst_port,my_packet->src_port,my_packet->ack,0,TH_RST,0,0,0,LIBNET_TCP_H,NULL,0,l,0);
        libnet_build_ipv4(LIBNET_IPV4_H+LIBNET_TCP_H+0,0,242,0,64,IPPROTO_TCP,0,dst_ip,src_ip,NULL,0,l,0);
        libnet_build_ethernet(my_packet->src_mac,my_packet->dst_mac,ETHERTYPE_IP,NULL,0,l,0);
        libnet_write(l);
        libnet_build_tcp(my_packet->src_port,my_packet->dst_port,my_packet->seq+my_packet->packet_size,0,TH_RST,my_packet->win,0,0,LIBNET_TCP_H,NULL,0,l,0);
        libnet_build_ipv4(LIBNET_IPV4_H+LIBNET_TCP_H+0,0,242,0,64,IPPROTO_TCP,0,src_ip,dst_ip,NULL,0,l,0);
        libnet_build_ethernet(my_packet->dst_mac,my_packet->src_mac,ETHERTYPE_IP,NULL,0,l,0);
        libnet_write(l);

    }

    return 0;
}
/*
void libnet_make_tcp_packet_nouse(struct packet_info *my_packet)
{
    libnet_t *l;
    u_long src_ip, dst_ip;
    // struct libnet_ether_addr *ptr_enet_src;
    src_ip = libnet_name2addr4(l,my_packet->src_ip,LIBNET_DONT_RESOLVE);
    dst_ip = libnet_name2addr4(l,my_packet->dst_ip,LIBNET_DONT_RESOLVE);

    l = libnet_init(LIBNET_LINK,"re0",errbuf);
    libnet_build_tcp(my_packet->src_port,my_packet->dst_port,my_packet->seq+1,htonl(1),0x14,htons(4500) + rand()%1000,0,0,LIBNET_TCP_H,NULL,0,l,0);
    libnet_build_ipv4(LIBNET_IPV4_H+LIBNET_TCP_H+0,0,242,0,64,IPPROTO_TCP,0,src_ip,dst_ip,NULL,0,l,0);
    libnet_build_ethernet(my_packet->dst_mac,my_packet->src_mac,ETHERTYPE_IP,NULL,0,l,0);
    libnet_write(l);
//send to dst
    l = libnet_init(LIBNET_LINK,"re0",errbuf);
    libnet_build_tcp(my_packet->dst_port,my_packet->src_port,my_packet->ack,my_packet->seq,0x14,0,0,0,LIBNET_TCP_H,NULL,0,l,0);
    libnet_build_ipv4(LIBNET_IPV4_H+LIBNET_TCP_H+0,0,242,0,64,IPPROTO_TCP,0,dst_ip,src_ip,NULL,0,l,0);
    libnet_build_ethernet(my_packet->src_mac,my_packet->dst_mac,ETHERTYPE_IP,NULL,0,l,0);
    libnet_write(l);
}

*/
void rst_tcp()
{
    struct packet_info packet_tmp;
    pthread_mutex_lock(mutex_for_cb_http_b);
    // while(cbIsEmpty(&cb_http_b))
    //{
    //  pthread_cond_wait(notEmpty_http_b, mutex_for_cb_http_b);
    //}
    cbRead(&cb_http_b, &packet_tmp);//packet temp~~~
    pthread_mutex_unlock(mutex_for_cb_http_b);
    pthread_cond_signal(notFull_http_b);
    public_fun_get_packet_info(packet_tmp.raw_packet,&packet_tmp);


}

/*
void rst_tcp_for_tcp_nouse()
{
    struct packet_info packet_tmp;
    while(0)
    {
        pthread_mutex_lock(mutex_for_cb_tcp_b);
        while(cbIsEmpty(&cb_tcp_b))
        {
            pthread_cond_wait(notEmpty_tcp_b, mutex_for_cb_tcp_b);
        }
        cbRead(&cb_tcp_b, &packet_tmp);//packet temp~~~
        pthread_mutex_unlock(mutex_for_cb_tcp_b);
        public_fun_get_packet_info(packet_tmp.raw_packet,&packet_tmp);
        pthread_cond_signal(notFull_tcp_b);
    }
}
*/
