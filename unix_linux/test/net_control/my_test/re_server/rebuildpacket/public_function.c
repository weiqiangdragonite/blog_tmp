///YS 一些共用的函数放在这里
#include <string.h>
#include <pcap/pcap.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include "packet_info.h"
#include "ringbuff.h"
#include <libnet.h>
#include <stdarg.h>
#include "config.h"
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <time.h>
#include <zlib.h>

extern int write_file_new(char * filename_tmp,long write_len,long offset,char *buff,long file_len,char *ip_or_iphost,int direct);
extern int uri_iconv (char *args);
extern  char filename_type[1024*1024];
extern int    put_flow_record_into_db(char *user_ip,long flow,int block);
extern int debug_open;
extern char net_addr[8];
struct user_flow userf[255];
char ipnat_block_list[1024*155];
extern char net_addr[8];


extern int sx;
char dst_db_ip[32]= {0};
char localip[32]= {0};
char dst_db_name[64]= {0};
char dst_db_user_name[64]= {0};
char dst_db_password[64]= {0};
extern char error_report_ip[32];
char http_host_name[128];
char http_packet_from[128];
char public_temp_buff[512];
char temp_sql_str[512];
extern char filepath[512];


/*********************
获取本地配置文件内容
参数1，filename是配置文件的位置
*********************/
int get_config_local_file(char *filename)
{
    FILE *fp=NULL;
    char c[1024];
    int i=0;
   fp=fopen("/mg/config","r");
    while(fgets(c,sizeof(c),fp)!=NULL)
    {
        for(i=0; i<64; i++)
        {
            if(c[i]=='\n') c[i]='\0';
        }

        if(strstr(c,"localip:"))
        {
            sprintf(localip,"%s",c+8);
        }

        if(strstr(c,"dbip:"))
        {
            sprintf(dst_db_ip,"%s",c+5);
        }
        if(strstr(c,"dbname:"))
        {
            sprintf(dst_db_name,"%s",c+7);
        }
        if(strstr(c,"dbusername:"))
        {
            sprintf(dst_db_user_name,"%s",c+11);
        }
        if(strstr(c,"dbpassword:"))
        {
            sprintf(dst_db_password,"%s",c+11);
        }
        if(strstr(c,"error_report_ip:"))
        {
            sprintf(error_report_ip,"%s",c+16);
        dbug("111111111 %s\n",error_report_ip);
        }
    }
    dbug("%s \n%s \n%s \n%s \n",dst_db_ip,dst_db_name,dst_db_user_name,dst_db_password);
    fclose(fp);
return 0;
}





/***********************
这个IPNAT是设置转换的条件，当系统知道某个人的流量超出了限制流量后，将会使用ARP欺骗，到另一个网关，并且，该网关会拦截该IP的所有包
***********************/
int ipnat_setting()
{
	return 0;
    //ipnat -CF -f /etc/ipnat.conf
    /*map em0 192.168.1.0/24 -> 192.168.100.129/32 portmap tcp/udp auto
#map em0 192.168.1.0/24 -> 192.168.126.138/32 portmap udp 30000:60000
map em0 192.168.1.211/32 -> 192.168.100.12/32
#map em0 192.168.1.0/24 -> 192.168.100.129/32
*/
//dbug("GGGGGGGGGGGGGGGGGGG %s\n",ipnat_block_list);
char tmp[1024*200]={0};
sprintf(tmp,"echo \"%spass in quick from any to any port = 53 keep state\r\n\
pass out quick from any to any port = 53 keep state\r\n\
pass in quick proto tcp from any to any\r\n\
pass out quick proto tcp from any to any\r\n\
block in quick proto udp from any to any port 2000 >< 65534\" > /etc/ipf.rules",ipnat_block_list);
system(tmp);
dbug("ipnat_block_list>>>>>>>>>>>>>>>>> %s \n",ipnat_block_list);
system("ipf -Fa -f /etc/ipf.rules");///YS 刷新我们的ipf规则
return 0;
}
//#define block_str "map em0 %s/32 -> 1.1.1.1/32\r\n"
#define block_str "block in quick from %s/32 to any\r\n"  ///YS IPF的阻断规则



/*********************************
把用户的流量信息写入到我们的数据库
**********************************/
int flow_count_insert_db()
{
    int i=0;
    char temp_str[255];
    memset(ipnat_block_list,0,sizeof(ipnat_block_list));
    for(i=0; i<255; i++)///YS 这个I是用户的IP最后一位，192。168.1.22，那么这个I就是22
    {

        if(userf[i].flow<1) continue;

        if(userf[i].flow>userf[i].limit && userf[i].limit>0 )///YS 如果超出了限制的流量
        {
            memset(temp_str,0,sizeof(temp_str));
            snprintf(temp_str,255,block_str,userf[i].ip);
            strcat(ipnat_block_list,temp_str);
            put_flow_record_into_db(userf[i].ip,userf[i].flow,1);///YS  userf[i].flow表示用户的总流量； 1，表示，需要阻断
        }
        else
        {
            put_flow_record_into_db(userf[i].ip,userf[i].flow,0);
        }
    }
    if(sx==1) {
		//dbug("111111111\n",sx);
//		exit(0);
	}
  ipnat_setting();///YS 对超过流量限制的用户进行阻断
    return 0;

}


/*********************************
计算用户的流量
参数1，packet_tmp，我们的包
**********************************/
int flow_count(struct packet_info * packet_tmp)
{
    //if(packet_tmp->packet_size>1600) exit(0);
    int ip1,ip2,ip3,ip4;
    if(strstr(packet_tmp->dst_ip,net_addr))
    {
        sscanf(packet_tmp->dst_ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);///YS获取IP最后一位，这个位就是数组里的直接应射
        if(strlen(userf[ip4].ip)<3)
        {
            sprintf(userf[ip4].ip,"%s",packet_tmp->dst_ip);
            userf[ip4].sub_net=ip3;
            userf[ip4].flow=userf[ip4].flow+packet_tmp->packet_size;///YS 用户的流量增加
          //   dbug("11111111111111111111size %d\n",packet_tmp->packet_size);
        }
        else///YS 如果这个IP已经在我们的流量统计数组里存在了
        {
            userf[ip4].flow=userf[ip4].flow+packet_tmp->packet_size;
         //    dbug("222222222222222222size %d  %d  %d\n",packet_tmp->packet_size,userf[ip4].flow,ip4);

        }
    }

    if(strstr(packet_tmp->src_ip,net_addr))
    {
        sscanf(packet_tmp->src_ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
        if(strlen(userf[ip4].ip)<3)
        {
            sprintf(userf[ip4].ip,"%s",packet_tmp->src_ip);
            userf[ip4].sub_net=ip3;
            userf[ip4].flow=userf[ip4].flow+packet_tmp->packet_size;
        //     dbug("33333333333333333333size %d\n",packet_tmp->packet_size);
        }
        else
        {
            userf[ip4].flow=userf[ip4].flow+packet_tmp->packet_size;
        //     dbug("4444444444444444444444size %d  %d  %d\n",packet_tmp->packet_size,userf[ip4].flow,ip4);
        }
    }

    return 0;
}


/**************************
数据转换函数，把字符FF，转换为真正的16进制
**************************/
int public_fun_str2num(char *str)
{
    char *p=str;
    int data=0;
    while (*p)
    {
        if (*p>='0' &&*p<='9')  data=(data<<4)+(*p-'0');  /*data<<4µÈ¼ÛÓÚdata*16*/
        else if (*p>='a' && *p<='f')  data=(data<<4)+(*p-'a'+10);
        else if (*p>='A' && *p<='F')  data=(data<<4)+(*p-'A'+10);
        else
        {
            //dbug("*c is not a hex character.\n",*p);
            return -1;
        }
        p++;
    }
    return data;
}

Bytef zip_destbuff[1000000];
const unsigned long iDLen = 1000000;


/***********************
HTTP解压函数（ZIP解压）
 参数 1.压缩数据
 参数 2.数据长度
 参数 3.解压数据
 参数 4.解压后长度
 ***********************/
int httpgzdecompress(Byte *zdata, uLong nzdata, uLong *ndata)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */
    memset(zip_destbuff,0,sizeof(zip_destbuff));
    static char dummy_head[2] =
    {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in  = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = zip_destbuff;
    if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
    while (d_stream.total_out < *ndata && d_stream.total_in < nzdata)
    {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if(err != Z_OK )
        {
            if(err == Z_DATA_ERROR)
            {
                d_stream.next_in = (Bytef*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK)
                {
                    return -1;
                }
            }
            else return -1;
        }
    }
    if(inflateEnd(&d_stream) != Z_OK) return -1;
    *ndata = d_stream.total_out;
    return 0;
}

/********************
这个1行的解压函数，现在没有使用了，
我在ZIP的官网上找了别的函数代替了
********************/
int UnzipHttpFile(Bytef *s,unsigned int iSLen,unsigned long *knlen/*,Byte *d,unsigned long * iDLen*/)
{
    int err ;
    z_stream d_stream;

    memset(&d_stream,0,sizeof(z_stream));
    memset(zip_destbuff,0,sizeof(zip_destbuff));
    d_stream.next_in = s;
    d_stream.avail_in = iSLen;
    d_stream.next_out =zip_destbuff;
    d_stream.avail_out = iDLen;

    err = inflateInit2(&d_stream,-MAX_WBITS);
    if(err != Z_OK)
        return err ;
    d_stream.next_in += 10L;
    err = inflate(&d_stream,Z_FINISH);
    if(err != Z_STREAM_END)
    {
        inflateEnd(&d_stream);
        return err == Z_OK ? Z_BUF_ERROR:err;
    }
    *knlen = d_stream.total_out;
    err = inflateEnd(&d_stream);
    return err;
}


/********************
判断文件名是否应该保存下来
参数1，filename是要判断的文件名
********************/
int  if_file_should_write( char *filename)
{
    if(strchr(filename,'?')) return 0;

    char file_type[64];
    char *p;
    p=strrchr(filename,'.');
    snprintf(file_type,sizeof(file_type),"%s",p);
    if(file_type[strlen(file_type)-1]==0x20)///YS 有的文件的文件名最后会有一个空格，把这个空格去了
    {
        file_type[strlen(file_type)-1]='\0';
    }

    if(strstr(filename_type,file_type)) ///YS 查找后缀
    {
        //s/asdfsdf/sasdfasdf.png?asdfsadf.png
        // dbug("::::::::::::::::::::::::::::::222222222222222 it %d\n",strstr(filename_type,file_type));
        return 1;
    }
    else
    {
        return 0;
    }
}


/********************
以前使用的，创建文件函数，现在没有使用了
********************/
long create_file_t(int file_len,char *filename,long offset,char *buff,int effect_len)
{
    if(!if_file_should_write(filename)) return 0;///文件名不在我们的审计范围
    long seek;
    FILE *stream;
    stream = fopen(filename,"r+");
    if(!stream)
    {
        stream = fopen(filename,"w");
    }
    if(!stream)return 0;////filename too long is a BUG!!!
    fseek(stream,0,SEEK_END);
    seek=ftell(stream);

    //dbug("seek is  %ld\n",seek);

    if(seek<offset)
    {
        char tmp[offset-seek];
        /*   fseek(stream,0,SEEK_END);
           seek=ftell(stream);
            */
        //dbug("  %d  %d %d\n",offset,seek, stream);
        if(stream)fwrite(tmp,offset-seek,1,stream);
        //dbug("seek2 is  %ld\n",ftell(stream));
    }


    if(offset+effect_len>file_len)
    {
        fseek(stream,offset,SEEK_SET);
        fwrite(buff,file_len-offset,1,stream);
        //  dbug("kkkkkkkkkkkkkkkkkkkkkk filelen is %ld   write len %ld \n",file_len,offset+effect_len);
    }
    else
    {

        //  dbug("tttttttttttttttttttttt filelen is %ld   write len %ld \n",file_len,offset+effect_len);
        fseek(stream,offset,SEEK_SET);
        //dbug("### buff >>> offset is %ld  and old offset is %ld  len is %ld\n",ftell(stream),offset,effect_len);
        fwrite(buff,effect_len,1,stream);
    }
    seek = ftell(stream);
    fclose(stream);
    //dbug("after i write the seek is : %ld\n",seek);
///get the offset
    return seek;

}


/********************
以前使用的，该函数是strstr的相反，可以从字符串的结尾查找
********************/
int public_fun_strrstr(char * source ,char * cmp,int len,int file_type_name_len,char *result_str)
{
    int i=0,j=0,k=0;
    char *p=NULL;
    char file_name[1024];
    int cmpstr_len=strlen(cmp);
    char hex[cmpstr_len/4];
    char tmp_hex[2];
    //source=source-cmpstr_len; pppppppppppppppppppp
    memset(file_name,0,1024);

    if(cmp[0]=='\\' && cmp[1]=='x' )
    {
        i=i+2;
        while(cmp[i])
        {
            if(cmp[i]=='\\' && cmp[i+1]=='x' )
            {
                i=i+2;
                continue;
            }
            tmp_hex[0]=cmp[i];
            tmp_hex[1]=cmp[i+1];
            hex[j]=public_fun_str2num(tmp_hex);
            i=i+2;
            j++;
        }
        memset(cmp,0,strlen(cmp));
        cmpstr_len=cmpstr_len/4;
        memcpy (cmp,hex,cmpstr_len);
    }
    i=0;
    j=0;

    for(i=0; i<len; i++)
    {
        p=memmem(source-i,cmpstr_len+i,cmp,cmpstr_len);
        if(p)//p找到了,可能是／，可能是filename   filename= asfasfasfd.jpg
        {

            k=0;
            for(  j=cmpstr_len; j<i+file_type_name_len; j++  )//01name.jpg
            {
                if(isprint(p[j]))
                {
                    file_name[k]=p[j];
                    k++;
                }
                else
                {
                    char a[3];
                    //sprintf(a,"?%X",(unsigned char)p[j]);
                    sprintf(a,"%%%X",(unsigned char)p[j]);
                    //sprintf(a,"%%%s",s);
                    //dbug("%s",a);
                    strcat(file_name,a);
                    k=k+3;
                }
            }

            // snprintf(file_name,i+1+file_type_name_len,"%s",p+cmpstr_len);
            if(cmp[0]==47&& !cmp[1])
            {
                // snprintf(file_name,i+1+file_type_name_len,"%s",p+cmpstr_len);
                if(strstr( file_name,"filename"  ))
                {
                    return  0;
                }
            }
            strcat(result_str,file_name);
            return 1;
        }

    }
    return 0;
}



/********************
* 获取包的有效信息
* 参数packet，原始包；
* 参数my_packet，我们自己的包
*
*********************/
int public_fun_get_packet_info(const u_char * packet,struct packet_info  *my_packet)
{
    struct tcphdr *tcp_header;// tcp header

    /***************************tCP  info  **************************/
    tcp_header= (struct tcphdr *)(packet+ sizeof(struct ether_header) + sizeof(struct ip));
    my_packet->dst_port=ntohs(tcp_header->th_dport);///YS 目标端口
    my_packet->src_port=ntohs(tcp_header->th_sport);///YS 源端口
    my_packet->ack=ntohl(tcp_header->th_ack);///YS ACK
    my_packet->seq=ntohl(tcp_header->th_seq);///YS SEQ
    return 0;
}



/********************
调试输出函数
参数与printf 一致
*********************/
void dbug(char *fmt,...)
{
    if(debug_open)
    {
        va_list ap;
        char string[256];
        va_start(ap,fmt);
        vsprintf(string,fmt,ap);
        if(strstr(string,"imp"))///YS imp == important
        {
            printf("\e[35mdebug info >>>>");
            printf(string+3);
            printf("\e[0m \n");
        }
        else
        {
            printf("debug info >>>>");
            printf(string);
            printf("\n");
            va_end(ap);
        }
    }
}



/****************************
*  查看是哪种类型的数据链路，并且确定其包头的偏移量
*  参数 pd是打开网卡后的句柄
*****************************/
int public_fun_datalink_off(pcap_t *pd)
{
    int offset = -1;
    switch (pcap_datalink(pd))
    {
    case DLT_EN10MB:
        offset = 14;
        break;
    case DLT_IEEE802:
        offset = 22;
        break;
    case DLT_FDDI:
        offset = 21;
        break;
#ifdef DLT_LOOP
    case DLT_LOOP:
#endif
    case DLT_NULL:
        offset = 4;
        break;
    default:
        dbug("unsupported datalink type\n");
        break;
    }
    return (offset);
}







/****************************
*  处理用户使用HTTP，POST文件的函数，用户在发送文件时，部份上传文件时会有该标志，当然也有一些是没有的
*  参数1，out_file_content是TCP流中用户发出的流
*	 参数2，是这个流的长度
*  参数3，是用户IP
*****************************/
int process_post_boundary( char * out_file_content, long out_file_len,char *ip)
{
    struct http_info http_info_tmp;
    http_info_tmp.file_len=0;
    char *p=NULL;
    char *p2=NULL;
    long int len_end=0,len_start=0;
    if(out_file_len)
    {
        //    dbug("%x%x%x%x  before get the post QMO the len is %d\n",out_file_content[0],out_file_content[1],out_file_content[2],out_file_content[3],out_file_len);
        p=(char *)memmem( out_file_content,out_file_len,"----Q",4);///YS boundary上传文件的特征，可以通过抓包发现
        if(p)
        {
            dbug("get the post QMO\n");
            ///YS 找到HTTP头了
            p2=(char *)memmem( out_file_content,out_file_len,"filename=\"",10 );///YS 找到文件名
            if(p2)
            {
                len_start=p2-out_file_content;
                p=strchr(out_file_content+len_start,'\r');
                if(p)
                {
                    len_end=p-out_file_content;
                    if(len_end-len_start-8>sizeof(http_info_tmp.filename))///YS 判断长度是否过长了
                    {
                        snprintf(http_info_tmp.filename,sizeof(http_info_tmp.filename),"%s",p2+10);
                    }
                    else
                    {
                        snprintf(http_info_tmp.filename,len_end-len_start-10,"%s",p2+10);
                    }

                    dbug("get the post filename %s   \n",http_info_tmp.filename);
                }
                if(http_info_tmp.filename)
                {
                    p=(char *)memmem( out_file_content+len_end,out_file_len-len_end,"\x0D\x0A\x0D\x0A",4 );///YS 找到HTTP头的结尾
                    if(p)
                    {
                        len_start=p-out_file_content+4;///YS 内容正文的开始
                        dbug("the lenstart is %ld  \n",len_start);
                        p2=(char *)memmem( out_file_content+len_start,out_file_len-len_start,"\x0D\x0A\x2D\x2D",4 );///YS 找到文件内容的结尾
                        if(p2)
                        {
                            len_end=p2-out_file_content;
                            dbug("get the post content end  %ld   \n",len_end);
                            dbug("content len is %ld\n",len_end-len_start);
                            http_info_tmp.file_len=len_end-len_start;
                        }
                        if(http_info_tmp.file_len>0)
                        {
                            write_file_new(http_info_tmp.filename,http_info_tmp.file_len,0,out_file_content+len_start,http_info_tmp.file_len,ip,0);///YS 生成文件
                        }
                    }
                }

            }
        }

    }



    return 0;
}



/*************************
把TCP流中的两个不同方向的流转换成临时文件，XXX_IN  XXXX_OUT,方便后面审计时对该文件进行处理
参数1，是要处理的流
参数2，是IN方向的流的长度
参数3，是OUT方向的流的长度
*************************/

int create_tmp_file( struct stream * s,int in_len,int out_len)
{

    if(1)
    {
        if(in_len)
        {

            char filename[64];
            memset(filename,0,sizeof(filename));
            snprintf(filename,sizeof(filename),"%s_in",s->stream_index);///YS 流的对应的临时文件
            FILE *stream_p_in=NULL;
            stream_p_in = fopen(filename,"a+");///YS 以追加的型式打开，没有的话就创建一个文件
            if(stream_p_in)
            {
                fwrite(s->istream,in_len,1,stream_p_in);///YS 写入文件
                fclose(stream_p_in);
            }
        }

    }
    if(2)
    {
        if(out_len>0)
        {
            char filename[64];
            memset(filename,0,sizeof(filename));
            snprintf(filename,sizeof(filename),"%s_out",s->stream_index);///YS 同上
            FILE *stream_p_out=NULL;
            stream_p_out = fopen(filename,"a+");
            if(stream_p_out)
            {
                fwrite(s->ostream,out_len,1,stream_p_out);
                fclose(stream_p_out);
            }
        }
    }
    return 0;
}


/*******************************
该函数是对分段下载文件的流进行处理，得出相应的文件名字和数据在文件中的偏移量，其型式如下：
///Content-Range: bytes 2736128-5482975/5482976    bytes 起始位-结束位/文件总长度
参数1，istream_header，其实是HTTP头
参数2，LEN是该HTTP头的长度
参数3，http_info_tmp是我们自定义的HTTP信息结构体，用于保存该函数分析后的结果
*******************************/
int get_http_download_filename_and_offset(char *istream_header,int len,struct http_info* http_info_tmp)
{
    char *p=NULL;
    char *p2=NULL;
    int len_start=0;
    int len_end=0;
    int i=0;
    char hex_file_name[1440];
    ///get file range
    p=(char *)memmem( istream_header,len,"Content-Range",12 );///YS 是否有该标志
    if(p)
    {
        len_start=p-istream_header;
        p2=strchr(istream_header+len_start,'\r');
        if(p2)
        {
            len_end=p2-p+1;
            if(len_end+2>sizeof(http_info_tmp->content_range))///YS 判断长度
            {
                snprintf(http_info_tmp->content_range,sizeof(http_info_tmp->content_range),"%s",istream_header+len_start);
            }
            else
            {
                snprintf(http_info_tmp->content_range,len_end,"%s",istream_header+len_start);
            }




        }
        ///Content-Range: bytes 2736128-5482975/5482976
        ///Content-Range: 278528-1112278/1112279

        long a=0;
        if(http_info_tmp->content_range[15] != 'b')///YS 这里要注意，有的HTTP返回的结果是带有“bytes”，有的是不带，这里要区分开来
        {
            sscanf(http_info_tmp->content_range,"Content-Range: %ld-%ld/%ld",&http_info_tmp->file_offset,&a,&http_info_tmp->file_len);///YS 找到长度和起始位了
        }
        else
        {
            sscanf(http_info_tmp->content_range,"Content-Range: bytes %ld-%ld/%ld",&http_info_tmp->file_offset,&a,&http_info_tmp->file_len);
        }

    }
    else ///YS Content-Length: 5482976
    {
        p=(char *)memmem( istream_header,len,"Content-Length",13 );///YS 查找文件总长度
        if(p)
        {
            len_start=p-istream_header;
            p2=strchr(istream_header+len_start,'\r');
            if(p2)///YS 找到文件总长度了
            {
                len_end=p2-p+1;

                if(len_end+2>sizeof(http_info_tmp->content_length))
                {
                    snprintf(http_info_tmp->content_length,sizeof(http_info_tmp->content_length),"%s",istream_header+len_start);
                }
                else
                {
                    snprintf(http_info_tmp->content_length,len_end,"%s",istream_header+len_start);
                }


            }
            sscanf(http_info_tmp->content_length,"Content-Length: %ld",&http_info_tmp->file_len);///YS 赋值到我们的HTTP结构体里
            http_info_tmp->file_offset=0;///YS 以Content-Length: 5482976这种型式出现的，其偏移量是0
        }
    }

    ///YS 获取文件名
    p=(char *)memmem( istream_header,len,"attachment; filename",18 );///YS 查找是否有该关键字，在HTTP头里出现的型式attachment; filename="zf2.pdf"
    if(p)
    {
        memset(http_info_tmp->filename,0,sizeof(http_info_tmp->filename));
        len_start=p-istream_header;
        p2=strchr(istream_header+len_start,'\r');
        if(p2)///YS 找到关键字了
        {
            len_end=p2-p+1;
            snprintf(http_info_tmp->attache_filename,1440,"%s",istream_header+len_start);
        }
        if(!sscanf(http_info_tmp->attache_filename,"attachment; filename=\"%s\"",http_info_tmp->filename))///YS获取文件名
        {
            dbug("no \"  ~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            sscanf(http_info_tmp->attache_filename,"attachment; filename=%s",http_info_tmp->filename);
        }
        else
        {
            http_info_tmp->filename[strlen(http_info_tmp->filename)-1]='\0';

            dbug("has \"  ~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        }

        ///因为文件名如果是中文的话，有时在下载时是以URL编码或UTF8编码出现，我们需要进行转换处理
        memset(hex_file_name,0,sizeof(hex_file_name));
        len_start=0;
        for(  i=0; i<strlen(http_info_tmp->filename); i++  )///YS 转换过程
        {
            if(isprint(http_info_tmp->filename[i]))///YS 看一下是否可以打印，也就是是否为乱码
            {
                hex_file_name[len_start]=http_info_tmp->filename[i];
                len_start++;
            }
            else///YS 不能打印的，我们以"%%%X"的型式，重新接回我们的FILENAME字符数组里
            {
                char a[3];
                sprintf(a,"%%%X",(unsigned char)http_info_tmp->filename[i]);
                strcat(hex_file_name,a);
                len_start=len_start+3;
            }
        }
        memset(http_info_tmp->filename,0,sizeof(http_info_tmp->filename));
        strcpy(http_info_tmp->filename,hex_file_name);
        uri_iconv(http_info_tmp->filename);///YS 这里进行编码转换
    }

    //get dst_ip
//        strcpy(http_info_tmp->user_ip,dst_ip);
    //get user_ip
    //     strcpy(http_info_tmp->user_ip,user_ip);
    //get time
    //  lt =time(NULL);
    // snprintf(http_info_tmp->time,32,"%s",ctime(&lt));
    return 0;
}

/*
char *strreplace(char *src, char *oldstr, char *newstr)
{
char *needle = strstr(src, oldstr);
if (needle) {
char *resultstr = (char*)malloc(strlen(src)+strlen(newstr)+1);
strncpy(resultstr, src, needle-src);
resultstr[needle-src]='/0';
strcat(resultstr, newstr);
strcat(resultstr, needle+strlen(oldstr));
needle=strstr(resultstr,oldstr);
if(needle!=NULL)
{
resultstr = strreplace(resultstr,oldstr,newstr);
}
return resultstr;
}
else { return src; }
}
*/



/***********************
获取数据包中的端口和IP等信息
参数1，是我们要处理的数据包
***********************/
int get_pro_and_ip(struct packet_info * packet_tmp)
{
    struct tcphdr *tcp_header;// tcp header
    struct ip * ip_header; //ip header
    struct udphdr *udp;

    ip_header = (struct ip *)(packet_tmp->raw_packet + sizeof(struct ether_header));
    {
        memset(packet_tmp->dst_ip,0,32);
        memset(packet_tmp->src_ip,0,32);

        strcpy(packet_tmp->dst_ip,inet_ntoa(ip_header->ip_dst));///YS 获取IP
        strcpy(packet_tmp->src_ip,inet_ntoa(ip_header->ip_src));

        if(ip_header->ip_p==6) ///YS 如果是tcp
        {
            packet_tmp->packet_type=Tcp;///YS 包的类型是tcp
            tcp_header= (struct tcphdr *)(packet_tmp->raw_packet+ sizeof(struct ether_header) + sizeof(struct ip));
            packet_tmp->packet_size=ntohs(ip_header->ip_len)-(tcp_header->th_off)*4 -(ip_header->ip_hl*4);///YS 数所包的有效长度(TCP有效负载)
            // packet_tmp->packet_size=packet_tmp->cap_size-(tcp_header->th_off)*4 -(ip_header->ip_hl*4)-14;
            //   dbug("packet len is %d\n",packet_tmp->packet_size);
            packet_tmp->offset = (packet_tmp->cap_size)-packet_tmp->packet_size;///YS tcp有效负载的数据的起始位
            //packet_tmp->offset = (tcp_header->th_off)*4 +(ip_header->ip_hl*4)+14;
//             dbug("packet header len is %d\n",packet_tmp->offset);
            packet_tmp->src_port=ntohs(tcp_header->th_sport);///YS 源端口
            packet_tmp->dst_port=ntohs(tcp_header->th_dport);
            packet_tmp->ack=ntohl(tcp_header->th_ack);///YS ACK
            packet_tmp->seq=ntohl(tcp_header->th_seq);///YS SEQ
            packet_tmp->flat=tcp_header->th_flags;///YS TCP的FLAG（PUS,ACK,FIN等）

        }
        else
        {
            if(ip_header->ip_p==17) ///YS 如果是udp包
            {
                //dbug("udp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                packet_tmp->packet_type=Udp;///YS包的类型为UDP
                udp=(struct udphdr*)(packet_tmp->raw_packet+ sizeof(struct ether_header) + sizeof(struct ip));
                packet_tmp->offset=8+(ip_header->ip_hl*4)+14;///YS 有效数据的起始位
                packet_tmp->packet_size=packet_tmp->cap_size-packet_tmp->offset;///YS 有效数据的总长
                packet_tmp->ip_id=ntohs(ip_header->ip_id);///YS IP头里的ID
                packet_tmp->src_port=ntohs(udp->uh_sport);///YS 源端口
                packet_tmp->dst_port=ntohs(udp->uh_dport);
            }
            else
            {
//                dbug("not tcp or udp \n");
                //not tcp or udp ys//
                return -1;///YS 来到这里的包，要么是IPV6，要么是IP报文
            }
        }
    }


    //strcpy(packet_tmp->dst_ip,inet_ntoa(ip_header->ip_dst));
    //ip_header->

    if(strstr(packet_tmp->dst_ip,net_addr))
    {
        packet_tmp->direction=d_in;
        snprintf(packet_tmp->stream,64,"%s#%d#%s#%d",packet_tmp->dst_ip,packet_tmp->dst_port,packet_tmp->src_ip,packet_tmp->src_port);///YS 创建该流的TCP流的名字
    }
    else
    {
        if(strstr(packet_tmp->src_ip,net_addr))
        {
            packet_tmp->direction=d_out;
            memset(packet_tmp->stream,0,64);
            snprintf(packet_tmp->stream,64,"%s#%d#%s#%d",packet_tmp->src_ip,packet_tmp->src_port,packet_tmp->dst_ip,packet_tmp->dst_port);///YS 创建该流的TCP流的名字

        }
    }

    return 0;
}






/*****************************
*  1，配置BPF的过滤规则
*  2，参数 handle是打开网卡后的句柄，filter_exp是BPF表达式，fp 是BPF表达式在编译成二进制后的指令的指针，net 是网卡掩码
*  3，出错返回-1
******************************/

int public_fun_bpf_config(pcap_t *handle,char *filter_exp,struct bpf_program fp,bpf_u_int32 net)
{
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1)///YS libpcap API，编译过滤条件
    {
        fprintf(stderr, "Couldn't parse filter %s: %s", filter_exp, pcap_geterr(handle));
        return -1;
    }
    if (pcap_setfilter(handle, &fp) == -1)///YS libpcap API，应用过滤条件
    {
        fprintf(stderr, "Couldn't install filter %s: %s", filter_exp, pcap_geterr(handle));
        return -1;
    }
    dbug("BPF configure success!");
    return 0;
}


