#include <stdio.h>
#include "packet_info.h"
#include "ringbuff.h"
#include "config.h"
#include <pcap/pcap.h>
#include <glib-2.0/glib.h>
#include <mysql/mysql.h>
#include <string.h>
#include <time.h>
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

extern int public_fun_strrstr(char * source ,char * cmp,int len,int file_type_name_len,char *result_str);
extern int write_file_new(char * filename_tmp,long write_len,long offset,char *buff,long file_len,char *ip_or_iphost,int direct);
extern int process_out_stream(char *in_file,char *out_file,char *ip);
extern int  if_file_should_write( char *filename);
extern int put_http_updownload_into_db(char * user_ip,char *filename,int u_or_d);
extern GHashTable *file_name_feature_list;
extern GHashTable *port_list;
extern GHashTable *file_name_ack_list;
extern GHashTable *ip_list;
extern struct pcre_info s_pcre[pcre_num];
extern char match_file_type[support_file_num][32];
int file_type_sum=0;
struct stream stream_array[20];
int array_count=0;
extern MYSQL *mysql;
extern char net_addr[8];
extern pthread_mutex_t *mutex_for_array;
char system_cmd[2048];
extern char date[24];
extern struct file_type file_header[Audit_count];

/*************************
获取上传文件的文件名
参数1，buff，是我们的上传的流，一般为POST
参数2，BUFF_LEN是流的大长
参数3，用户IP
*************************/
int upload_file_name(char *buff,int buff_len,char *ip)
{
    int ovector[OVECCOUNT];
    int len_end=0,len_start=0;
    int i=0,j=0,rcCM=0;
    char *p;
    for(i=0; i<Audit_count; i++)
    {
        p=(char *)memmem( buff,buff_len,file_header[i].filetype,strlen(file_header[i].filetype));///YS file_header[i].filetype是.jpg  .rar  .doc等
        if(p)
        {
            len_end=p-buff;
            dbug("upload file name type got !!!>>>>>>>>>>>>>> %s   len start is %d\n",file_header[i].filetype,len_end);
            for(j=0; j<500; j++) ///YS 从.JPG的位置向前找6个，看有没有NAME，一共找30次，因为型式一般为name:XXXX.JPG  或者是 filename: xxx.jpg
            {
                if(6*j>len_end)
                {
                    dbug("cant find the name=!!!!!!!!!!\n");
                    break;
                }
                p=(char *)memmem( buff+len_end-6*j,6*j,"name",4);
                if(p)
                {
                    dbug(" find the name=!!!!!!!!!!\n");
                    char file_name[128];
                    int len=0;
                    len_start=p-buff;
                    len=len_end-len_start+strlen(file_header[i].filetype)-5;

                    if(len+2>sizeof(file_name))
                    {
                        snprintf(file_name,sizeof(file_name),"%s",p+6);
                    }
                    else
                    {
                        snprintf(file_name,len,"%s",p+6);
                    }

                    //if(memmem( file_name,strlen(file_name),"ilename",7)) return 0;
                    if(file_name[0]==0x0D||file_name[0]==0x0A) file_name[0]='_';///YS 因为该法的通用性好，但会有一些特别，像有的文件名的开头会有回车换行的字符，所以我们要把他替换掉
                    if(file_name[1]==0x0D||file_name[1]==0x0A) file_name[1]='_';

                    if(file_name[2]==0x0D||file_name[2]==0x0A) file_name[2]='_';

                    if(file_name[3]==0x0D||file_name[3]==0x0A) file_name[3]='_';



                    dbug("get the name= #############  filename len is %d   %s\n",len,file_name);
                    ///YS find the pcre
                    {
                        rcCM = pcre_exec(file_header[i].p_pcre, NULL, buff, buff_len, 0, 0, ovector, OVECCOUNT);  ///YS 匹配pcre编译好的模式，成功返回正数，失败返回负数
                        if (rcCM==PCRE_ERROR_NOMATCH )
                        {
                            dbug("upload file no match !!!!\n");
                            return 0;
                        }
                        if(rcCM>=0)
                        {
                            //dbug("match  it  content start   %d!!! %x |%x |%x |%x \n",ovector[0],buff[ovector[0]],buff[ovector[0]+1],buff[ovector[0]+2],buff[ovector[0]+3]);//start of content;
                            dbug("match it %s filelen is %d\n",file_name,buff_len-ovector[0]);
                            write_file_new(file_name,buff_len-ovector[0],0,buff+ovector[0],buff_len-ovector[0],ip,4);
                            return 0;

                        }

                    }
                    ///YS end of the pcre
                }
            }
        }
    }
    return 0;
}

/**********************
已经没使用该方法来或得文件名了
***********************/
int point_name(struct packet_info * packet_tmp)
{
    char *p=NULL;
    char start[32];
    char tmp_behavior_type[32];
    int i,j=0,file_type_name_len,effect_len;
    char filename[1024];

    for(i=0; i<3; i++)
    {
        snprintf(tmp_behavior_type,sizeof(tmp_behavior_type),"%s%d",packet_tmp->behavior_type,i);
        p=g_hash_table_lookup(file_name_feature_list, tmp_behavior_type);
        if(p)//http0  http1  ftp tftp and so on
        {
            char *p2;
            p2=strchr(p,'#');
            if(p2-p-3>sizeof(start))
            {
                snprintf(start,sizeof(start),"%s",p+6);
            }
            else
            {
                snprintf(start,p2-p-5,"%s",p+6);// "/"  "filename="  "\x00\x01" and so on
            }


            for(j=0; j<file_type_sum; j++)
            {
                file_type_name_len=strlen(match_file_type[j]);
                p=memmem( packet_tmp->raw_packet,packet_tmp->cap_size,match_file_type[j],file_type_name_len  );
                if(p)
                {
                    effect_len=p-(char *)packet_tmp->raw_packet;
                    //ys i got the p,but i need to find the start str ys//
                    if  (public_fun_strrstr(p,start,effect_len,file_type_name_len,filename))
                    {
                        return 0;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    continue;
                }
            }
        }
    }
    return 0;
}


/**********************
处理TCP流
参数1，是我们的自定义的TCP流结构体
***********************/
int process_stream(struct stream * s)
{
    char tmp_file_name_in[1024];
    char tmp_file_name_out[1024];
    snprintf(tmp_file_name_in,sizeof(tmp_file_name_in),"%s_in",s->stream_index);
    snprintf(tmp_file_name_out,sizeof(tmp_file_name_out),"%s_out",s->stream_index);
    if(!strcmp(s->pro,"http"))
    {
        process_out_stream(tmp_file_name_in,tmp_file_name_out,s->user_ip);///YS 名字虽然是”process_out_stream“，但是这个函数会，处理我们的流，包括进和出的
    }
    if(!strcmp(s->pro,"smtp"))
    {
        dbug(" SMTP##################################\n")  ;

        smtp(tmp_file_name_out,tmp_file_name_in,s->user_ip);///YS sendemail,so we use out_strema

    }
    if(!strcmp(s->pro,"pop3"))
    {
        dbug(" pop3##################################\n")  ;
        pop3(tmp_file_name_in,s->user_ip);
    }

    unlink(tmp_file_name_in);///YS 删除临时文件
    unlink(tmp_file_name_out);///YS 删除临时文件
    return 0;
}


/**********************
跟据协议，从哈希表里找到相应的端口，如果是有固定端口协议的话，如FTP，TELNET
参数1，是我们自定义的网络数据包
***********************/
int find_pro_from_hash(struct packet_info * packet_tmp)
{
    char port_tmp[8];
    memset(port_tmp,0,sizeof(port_tmp));
    char *p=NULL;
    char *p_for_strdup =NULL;
    char *p_for_strdup_2=NULL;

    snprintf(port_tmp,sizeof(port_tmp),"%d",packet_tmp->src_port);///YS 先查找目标端口，要是目标端口没找到，就找源端口

    p=g_hash_table_lookup(port_list, port_tmp);
    if(p)///YS 如果找到了
    {
        if(p[0]==50 || p[0]==51)//50的 ascii对应数字2，或者3
        {
            strcpy(packet_tmp->behavior_type,p+2);///YS 字段的值是2TFTP，加2的话，可以直接拿到TFTP，而不是2TFTP
            return 1;
        }
        else
        {
            strcpy(packet_tmp->behavior_type,p);
            return 1;
        }
    }
    else ///YS 如果目标端口没有找到，找源端口
    {
        snprintf(port_tmp,sizeof(port_tmp),"%d",packet_tmp->dst_port);
        p=g_hash_table_lookup(port_list, port_tmp);
        if(p)
        {
            if(p[0]==50)///YS 50  ---> 2 ascii，这个标志识用于TFTP这种动态端口的
            {
                snprintf(port_tmp,sizeof(port_tmp),"%d",packet_tmp->src_port);
                p_for_strdup=g_strdup(port_tmp);
                p_for_strdup_2=strdup(p+2);
                g_hash_table_insert(port_list,p_for_strdup,p_for_strdup_2);///YS 这个端口找到了，那么两个端口都要放到HASH表里，像TFTP的协议，只有一个59的端口是固定的，但是另一个端口是动态的
                strcpy(packet_tmp->behavior_type,p+2);
                return 1;
            }
            else
            {
                if(p[0]==51)//50  ---> 3 ascii，这个标志只适用固定IP的，端口都是动态的
                {
                    if(!strstr(packet_tmp->dst_ip,net_addr))
                    {
                        p_for_strdup=g_strdup(packet_tmp->dst_ip);
                        p_for_strdup_2=g_strdup(p+2);
                        g_hash_table_insert(ip_list,p_for_strdup,p_for_strdup_2);///ys 放到IP表里面去  ，因后继的传送文件用的端口，都是动态的，所以用IP来做定断，像FTP ys//
                        strcpy(packet_tmp->behavior_type,p+2);
                        return 1;
                    }
                }
                else
                {
                    strcpy(packet_tmp->behavior_type,p);
                    return 1;
                }
            }
        }
        else///ys  如果两个端口都没有找到的话，那有可能就是固定IP的,再就找一下IP  hash   ys//
        {
            p=g_hash_table_lookup(ip_list, packet_tmp->dst_ip);
            if(p)
            {
                strcpy(packet_tmp->behavior_type,p);
                return 1;
            }
            else///ys  如果到最后，端口与IP都找不到 ，就返回一个Unknow
            {
                strcpy(packet_tmp->behavior_type,"Unknow");
                return 0;
            }
        }
    }
    return 0;
}




/**********************
审计内容如果是需要创建文件的话，则使用该函数。
参数1，是文件名
参数2，要写入的长度
参数3，写入时如果有偏移量的话，那就从这OFFSET开始
参数4，文件的内容
参数5，文件的长度
参数6，是IP，还HOST，因为每个用户都会有自己的文件夹，文件夹名字以IP为准，如果是HOST的话则以QQ.MAIL;163.MAIL等命名
参数7，只针对是上传或下载的，后来多了一个读邮件的但读邮件是自己独立一个写文件的函数，所以现在划分为：1是下载，0和4是上传
***********************/
int write_file_new(char * filename_tmp,long write_len,long offset,char *buff,long file_len,char *ip_or_iphost,int direct)
{
    char dir[1024];
    char filename[2024];
    if(direct==1)///YS 1是下载
    {
        snprintf(filename,sizeof(filename),tmp_download_file_path,ip_or_iphost,date,filename_tmp);
        snprintf(dir,sizeof(dir),tmp_download_file_path,ip_or_iphost,date,"");
    }
    if(direct==0)///YS 0是上传
    {
        snprintf(filename,sizeof(filename),tmp_upload_file_path,ip_or_iphost,date,filename_tmp);
        snprintf(dir,sizeof(dir),tmp_upload_file_path,ip_or_iphost,date,"");
    }
    /* if(direct==3)///YS 3是油件
    {
        snprintf(filename,sizeof(filename),tmp_email_file_path,ip_or_iphost,filename_tmp);
          snprintf(dir,sizeof(dir),tmp_email_file_path,ip_or_iphost,date,"");
      }*/
    if(direct==4)///YS 4是上传，正则式过来的
    {
        snprintf(filename,sizeof(filename),tmp_upload_file_path,ip_or_iphost,date,filename_tmp);
        snprintf(dir,sizeof(dir),tmp_upload_file_path,ip_or_iphost,date,"");
    }
    if(direct!=4)///YS 因为正则式拿到的文件名是绝对有效，而1，0过来的文件名是我们需要判断一下，在不在我们的审计范围
        if(!if_file_should_write(filename)) return 0;
//     dbug(">>>>>>>>>>>>>>>>>>>>>>>>>  file_len %ld  >>>>> %s\n",file_len-1,filename);

    FILE *stream;
    stream = fopen(filename,"r+");
    if(!stream)
    {
        if(file_len)
        {
            if(access(dir,0)<0)///YS 判断文件的路径是否存在，不存在的话，先建立该文件路径
            {
                char mkdir_str[64];
                sprintf(mkdir_str,"mkdir -p %s",dir);
                system(mkdir_str);
                dbug("dir not exiest \n");
            }
            snprintf(system_cmd,sizeof(system_cmd),system_str,filename,file_len);

            system(system_cmd);///YS 创建一个空文件
            stream = fopen(filename,"r+");
        }
        else
        {
            stream = fopen(filename,"w");
        }
    }
    if(!stream)return 0;

    if(offset)///YS 如果有偏移量，则从这里写起
    {
        dbug(" fseek %ld #####   %ld\n",offset,write_len);
        fseek(stream,offset,SEEK_SET);
        fwrite(buff,write_len,1,stream);
    }
    else
    {
        dbug(" fseek start #####   %ld\n",write_len);
        fseek(stream,0L,SEEK_SET);
        fwrite(buff,write_len,1,stream);
    }
    fclose(stream);
    if(direct!=3)///YS 如果不是邮件过来的，都是上传和下载的，那就把记录写入到数据库里
        put_http_updownload_into_db(ip_or_iphost,filename,direct);
    return 0;
}
