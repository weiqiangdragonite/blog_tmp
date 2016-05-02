#include <iconv.h>
#include "config.h"
#include "ringbuff.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
extern CircularBuffer cb_rawpacket;
extern pthread_mutex_t *mutex_for_cb;
extern pthread_cond_t *notFull_cb, *notEmpty_cb;
extern int get_pro_and_ip(struct packet_info * packet_tmp);
extern int find_pro_from_hash(struct packet_info * packet_tmp);
extern int remove_stream(struct packet_info *packet_tmp);
extern int store_packet(struct packet_info *packet_tmp2);


/**************************
获取一个包，并且创建要审计的流
**************************/
int get_syn_and_build_stream( )
{
    struct packet_info my_packet;
    memset(&my_packet,0,sizeof(struct packet_info));
    while(1)
    {
        pthread_mutex_lock(mutex_for_cb);
        if(cbIsEmpty(&cb_rawpacket))
        {
//            dbug("empty and wait \n\n");
            pthread_cond_wait(notEmpty_cb,mutex_for_cb);
        }
        cbRead(&cb_rawpacket, &my_packet);///YS 获取一个packet
        pthread_mutex_unlock(mutex_for_cb);
        pthread_cond_signal(notFull_cb);

        //if the syn packet
        if(get_pro_and_ip( &my_packet));
        flow_count(&my_packet);///YS 计算用户的流量
        // dbug("!!!!!!!!!!   %d\n",my_packet.cap_size);
        if(my_packet.cap_size>62)process_im(&my_packet);

        if(!find_pro_from_hash(&my_packet))///YS 查一下哈希表，得出该数据包的协议，是http or tftp or ftp or smtp or pop3
        {
            continue;
        }

        if(my_packet.packet_type==Udp)///YS 没有找到port时，就再看一下是不是UDP，是的话就处理一下UDP
        {
//            dbug("udp ~~~~~~~~~ %s\n",my_packet.behavior_type);
            udp_process(&my_packet);
        }


        if(my_packet.packet_type==Tcp)///YS 如果该包是TCP的话
        {
            if((my_packet.flat & TH_FIN)||(my_packet.flat & TH_RST) )///YS remove the tcp stream
            {
                remove_stream(&my_packet);
            }

            if((my_packet.flat & TH_SYN)||(my_packet.flat & TH_ACK))///YS build the tcp stream
            {

                store_packet(&my_packet);
            }

        }
    }
}

