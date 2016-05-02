///YS UDP审计文件的内容的过程如udp_process函数所示，一个包来了，
///YS 如果这个包里有文件头的类型，那我们就为该流创建一个文件，以后每次遇到这个流的数据包，我们就直接把数据包写入到文件里

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
#include <unistd.h>
#include "TCP_funtion.h"
#include <pthread.h>
#include <time.h>

extern struct file_type file_header[Audit_count];
extern struct pro_header_offset pro_header_offset_entity[32];
extern char date[24];
extern char  net_addr [8];
extern GHashTable *udp_list;

/**************************
插入把UDP的流的索引放入哈希表，KEY：流的名字+方向，VALUE：时间+文件类型
参数1：UDP数据包
参数2：文件类型的数组的索引
**************************/
int udp_insert_hash_list( struct packet_info * packet_tmp,int file_type)
{
    char filename[1024];
    char value[1024];

    dbug("::::::::::::::::::  time  is %d\n",packet_tmp->time);
    snprintf(value,1024,"%d%s",packet_tmp->time,file_header[file_type].filetype);
    if(packet_tmp->direction)
    {
        snprintf(filename,sizeof(filename),"%s_in",packet_tmp->stream);
    }
    else
    {
        snprintf(filename,sizeof(filename),"%s_out",packet_tmp->stream);
    }

    g_hash_table_insert(udp_list,g_strdup(filename),g_strdup(value));///YS 可输出KEY，VALUE查看值
    return 0;
}

/***********************
TFTP专用的，根据TFTP的数据包，我们可以看到其协议是把回车跟换行转换了，
0x0D换成0X0D与0x00等，该函数就是把其数据 进行还原
参数1，是要换转的内容
参数2，是内容的长度
***********************/
int tftp_replace_str(char *str,int len)
{
    dbug("*************999999  len %d\n",len);
    int i,j;
    unsigned char new_str[len];
    for(i=0; i<len; i++)
    {
        if(str[i]==0x0D&&str[i+1]==0x00)
        {
            new_str[j]=str[i];
            i++;
            j++;
            continue;
        }
        else
        {
            if(str[i]==0x0D&&str[i+1]==0x0A)
            {
                new_str[j]=str[i+1];
                i++;
                j++;
                continue;
            }
            else
            {
                new_str[j]=str[i];
                j++;
            }
        }
    }
    memset(str,0,len);
    memcpy(str,new_str,j);
    dbug("*************000000000  len %d\n",j);
    return len-j;
}




/***********************
UDP写文件，跟据filetype的值，可以知道是哪个文件类型，然后把有效的数据内容直接写入文件
参数1，UDP数据包
参数2，是确定文件类型
***********************/
int udp_write_file(struct packet_info * packet_tmp,char *filetype)
{
    char filename[1024];
    char dir[1024];
    FILE *stream;
    int j,len_tftp=0;
    if(strstr(packet_tmp->src_ip,net_addr))
    {
        snprintf(dir,sizeof(dir),"/var/udp/%s/%s/",packet_tmp->src_ip,date);
    }
    else
    {
        snprintf(dir,sizeof(dir),"/var/udp/%s/%s/",packet_tmp->dst_ip,date);
    }

    if(access(dir,0)<0)
    {
        char mkdir_str[64];
        sprintf(mkdir_str,"mkdir -p %s",dir);
        system(mkdir_str);
        dbug("dir not exiest \n");
    }

    if(packet_tmp->direction)
    {
        snprintf(filename,sizeof(filename),"%s%s_in%s",dir,packet_tmp->stream,filetype);
    }
    else
    {
        snprintf(filename,sizeof(filename),"%s%s_out%s",dir,packet_tmp->stream,filetype);
    }
    for(j=0; j<32; j++)
    {
        if(strlen(pro_header_offset_entity[j].pro_name)>0)
        {
            if(!strcmp(pro_header_offset_entity[j].pro_name,packet_tmp->behavior_type))///YS 查一下是哪种协议，以确定文件的有效数据的起始位等
            {
                break;
            }
        }
    }

    if(pro_header_offset_entity[j].offset_type==1)
    {
        if(!strcmp(packet_tmp->behavior_type,"tftp"))
        {
            ///替换OXOD
            len_tftp=tftp_replace_str(packet_tmp->raw_packet+packet_tmp->offset,packet_tmp->cap_size);

        }
        stream = fopen(filename,"a+");///YS 直接写入该文件就行了，因为我们以经找到该流是被创建过文件的
        fwrite(packet_tmp->raw_packet+packet_tmp->offset+pro_header_offset_entity[j].offset_len,
               packet_tmp->packet_size-pro_header_offset_entity[j].offset_len-len_tftp,1,stream);
        fclose(stream);
    }

    dbug("$$$$$$$$$$$$$$$$4   write file\n");

    return 0;
}

/***********************
先找一下哈希表有没有对应的包，有的话就不用找文件头了。
参数1，是UDP数据包
***********************/
int udp_find_hash_list(struct packet_info * packet_tmp)
{
    char *p;
    char filename[1024];
    char filetype[16];
    int tmp;
    if(packet_tmp->direction)
    {
        snprintf(filename,sizeof(filename),"%s_in",packet_tmp->stream);
    }
    else
    {
        snprintf(filename,sizeof(filename),"%s_out",packet_tmp->stream);
    }
    if((p=g_hash_table_lookup(udp_list,filename)))///YS 找一下UDP是否已经有的相应的文件名
    {
        sscanf(p,"%d%s",&tmp,filetype);///YS 获取文件的类型
        udp_write_file(packet_tmp,filetype);///YS 写文件
    }
    return 0;
}



/***********************
UDP创建文件，跟据i的值，可以知道是哪个文件类型，接下来创建文件（stream——index+in or out.jpg),这个创建的文件名以“流+IN/OUT.文件类型”来命名
参数1，UDP数据包
参数2，是确定文件类型
***********************/
int udp_create_file(struct packet_info * packet_tmp,int i)
{
    char filename[1024];
    char dir[1024];
    FILE *stream;
    int j,len_tftp=0;
    if(strstr(packet_tmp->src_ip,net_addr))
    {
        snprintf(dir,sizeof(dir),"/var/udp/%s/%s/",packet_tmp->src_ip,date);///YS 确定目录
    }
    else
    {
        snprintf(dir,sizeof(dir),"/var/udp/%s/%s/",packet_tmp->dst_ip,date);
    }

    if(access(dir,0)<0)///YS 判断该路径是否存在了
    {
        char mkdir_str[64];
        sprintf(mkdir_str,"mkdir -p %s",dir);
        system(mkdir_str);///YS 不存在的话，就创建该路径
        dbug("dir not exiest \n");
    }

    if(packet_tmp->direction)
    {
        snprintf(filename,sizeof(filename),"%s%s_in%s",dir,packet_tmp->stream,file_header[i].filetype);///YS 创建文件名
    }
    else
    {
        snprintf(filename,sizeof(filename),"%s%s_out%s",dir,packet_tmp->stream,file_header[i].filetype);
    }
    dbug("udp>>>>>>>>    %s \n  %d  %d  %d\n",filename,packet_tmp->offset,packet_tmp->cap_size,packet_tmp->packet_size);

    for(j=0; j<32; j++)
    {
        if(strlen(pro_header_offset_entity[j].pro_name)>0)
        {
            if(!strcmp(pro_header_offset_entity[j].pro_name,packet_tmp->behavior_type))///YS UDP的话我们使用pro_header_offset_entity结构体来处理文件数据的起始位，与有效的文件长度
            {
                dbug("$$$$$$$$$$$$$$$$4    %d\n",pro_header_offset_entity[j].offset_len);
                break;
            }
        }
    }

    if(pro_header_offset_entity[j].offset_type==1)///YS OFFSET类型，有固定长度，有特征字两种
    {
        if(!strcmp(packet_tmp->behavior_type,"tftp"))///YS TFTP是固定长度的，在UDP负载的有效数据的前4个字节是用于控制数据包的顺序的
        {
            ///YS 替换OXOD
            len_tftp=tftp_replace_str(packet_tmp->raw_packet+packet_tmp->offset,packet_tmp->cap_size);

        }
        if(access(filename,0)<0)///YS 如果文件名不存在，我们使用W+来打开文件
        {
               stream = fopen(filename,"w+");
        }
        else return 0;
        fwrite(packet_tmp->raw_packet+packet_tmp->offset+pro_header_offset_entity[j].offset_len,
               packet_tmp->packet_size-len_tftp-pro_header_offset_entity[j].offset_len,1,stream);
               dbug("888888888   %d  %d   %d\n",packet_tmp->packet_size,len_tftp,pro_header_offset_entity[j].offset_len);///YS 创建文件
        fclose(stream);
    }
    return 0;
}

/*************************
利用正式则，查找UDP中的文件的文件头
参数1，是UDP数据包
**************************/
int udp_find_file_header(struct packet_info * packet_tmp)
{
    int ovector[OVECCOUNT];
    int i=0, rcCM=0;
    for(i=0; i<OVECCOUNT; i++)
    {
        rcCM = pcre_exec(file_header[i].p_pcre, NULL, packet_tmp->raw_packet, packet_tmp->packet_size, 0, 0, ovector, OVECCOUNT);  ///YS 匹配pcre编译好的模式，成功返回正数，失败返回负数
        if (rcCM==PCRE_ERROR_NOMATCH )
        {
//            dbug("no match !!!!\n");
            return 0;
        }
        if(rcCM>=0)
        {
            dbug("match ########################  %d!!!!\n",i+1);
            return i+1;
        }
    }
    return 0;
}

/***************************
udp包处理函数
参数1，是UDP数据包
***************************/
int udp_process(struct packet_info * packet_tmp)
{
    int ret=0;
    ///YS 先找一下哈希表有没有对应的包，有的话就不用找文件头了。
    if((ret=udp_find_hash_list(packet_tmp)))
    {
        ///YS 找到了后，那就说明文件已经存在了，果断写入那个文件

        ///YS 还要更新一下hash_list的时间
    }
    else
    {
        ///YS 再找一下有没有相应的文件头，有就把包的STREAMINDEX和方向记录下来
        if((ret=udp_find_file_header(packet_tmp)))
        {
            ///YS 跟据RET的值，可以知道是哪个文件类型，接下来创建文件（stream——index+in or out.jpg)
            udp_create_file( packet_tmp,ret-1);
            ///YS 创建完文件后，就插入到hash表里key:streamidex+in or out  ,value:time_now,RET是文件类型
            udp_insert_hash_list( packet_tmp,ret-1);
            //exit(0);
        }
    }
    return 0;
}
