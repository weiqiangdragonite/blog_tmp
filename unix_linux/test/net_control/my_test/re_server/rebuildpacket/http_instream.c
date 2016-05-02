#include <stdio.h>
#include "time.h"
#include "packet_info.h"
#include "config.h"
#include <mysql.h>
#include <string.h>

extern int http_enable;
extern int get_http_download_filename_and_offset(char *istream_header,int len,struct http_info* http_info_tmp);
extern int write_file_new(char * filename_tmp,long write_len,long offset,char *buff,long file_len,char *ip_or_iphost,int direct);
//int  process_http_header(const char *header_buff,   struct http_info * http_info_tmp,int direct)
extern int put_http_host_into_db(char * user_ip,char *host,char *referer);


/**************************
处理HTTP头
参数1，HTTP头的BUFF指针
参数2，http的结构体
参数3，TCP流的方向
参数4，用户IP
***************************/
int  process_http_header(const char *header_buff,   struct http_info * http_info_tmp,int direct ,char *ip)
{
    if(direct==1)///YS 200，304等这些都是IN方向才会有的
    {
        char *p;
        p=(char*)memmem( header_buff,http_info_tmp->http_hl,"304 Not Modified",14 );
        if(p)
        {
            //   dbug(">>>>>>>>>>>>>>>>>>>>>>>>3 0 4\n");
            return  304;
        }
        else
        {
            p=(char*)memmem( header_buff,http_info_tmp->http_hl,"200 OK",6 );
            if(p)
            {
                //    dbug(">>>>>>>>>>>>>>>>>>>>>>>>2 0 0 ok\n");
                return 200;
            }

            p=(char*)memmem( header_buff,http_info_tmp->http_hl,"206 Par",6 );
            if(p)
            {
                //     dbug(">>>>>>>>>>>>>>>>>>>>>>>>206 Par\n");
                return 206;
            }
        }
    }
    if(direct==2)///如果是OUT方向的，我们就找HOST和REFERER，和用户GET了什么文件
    {
        char *p,*p2;
        int len_start,len_end;
        ///YS 获取URL
        p=(char *)memmem( header_buff,http_info_tmp->http_hl,"Host: ",6 );
        if(p)
        {
            len_start=p-header_buff;
            p2=strchr(header_buff+len_start,'\r');
            if(p2)
            {
                //len_end=p2-p+1;
                len_end=p2-p;

                if(len_end-5>sizeof(http_info_tmp->url))
                {
                    snprintf(http_info_tmp->url,sizeof(http_info_tmp->url),"%s",header_buff+len_start+6);
                }
                else
                {
                    snprintf(http_info_tmp->url,len_end-5,"%s",header_buff+len_start+6);
                }


                if(http_enable)
                {
                    ///YS get the referer
                    p=(char *)memmem( header_buff,http_info_tmp->http_hl,"Referer:",8 );
                    if(p)
                    {
                        len_start=p-header_buff;
                        p2=strchr(header_buff+len_start,'\r');
                        if(p2)
                        {
                            // len_end=p2-p+1;
                            len_end=p2-p;

                            if(len_end-13>sizeof(http_info_tmp->referer))
                            {
                                snprintf(http_info_tmp->referer,sizeof(http_info_tmp->referer),"%s",header_buff+len_start+16);
                            }
                            else
                            {
                                snprintf(http_info_tmp->referer,len_end-15,"%s",header_buff+len_start+16);
                            }

                        }
                        //dbug(" %s\n",http_info_tmp->url );
                        //dbug(" %s\n",http_info_tmp->referer );
                        if(strlen(http_info_tmp->referer)>5&& strlen(http_info_tmp->url)>5)
                        {
 //                           dbug("%s   %s\n",http_info_tmp->url,http_info_tmp->referer);
                            put_http_host_into_db(ip,http_info_tmp->url,http_info_tmp->referer);///YS 把URL，REFERE写入数据库
                        }
                    }
                }
            }

            ///YS 如果找到GET和HTTP字眼，那就说明用户在浏览或者下载
            p=strstr(header_buff,"GET ");
            if(p)
            {
                len_start=p-header_buff;
                p2=strstr(header_buff+len_start," HTTP");
                if(p2)
                {
                    char str[2024];
                    if(p2-p>sizeof(str))
                    {
                        snprintf(str,sizeof(str),"%s",header_buff+len_start+4 );
                    }
                    else
                    {
                        snprintf(str,p2-p-3,"%s",header_buff+len_start+4 );
                    }

                    strcat(http_info_tmp->url,str);
                }
            }
            ///YS get filename ，如http://www.xxx.x.x.com/aaa.doc，从后面数起，遇到第一个/为止
            p=strrchr(http_info_tmp->url,'/');
            if(p)
            {
                snprintf(http_info_tmp->filename,512,"%s",p+1);
            }
        }
    }

    if(direct==3)///YS for out strem  POST主要是拿文件名和结束
    {
        char *p,*p2;
        int len_start;
        ///get url
        p=(char *)memmem( header_buff,http_info_tmp->http_hl,"boundary=",9 );///YS boundary，上传时的特征
        if(p)
        {
            len_start=p-header_buff;
            p2=strchr(header_buff+len_start,'\r');
            if(p2)
            {
                return 3;
            }
        }
    }

    return  0;
}


/***********************
处理IN—STREAM的函数
参数1，TCP流INSTREAM的BUFF
参数2，文件名
参数3，BUFF的长度
参数4，用户IP
***********************/
int process_in_stream(char * in_file_content,char  *filename,long in_file_len,char *ip)
{
    struct http_info http_info_tmp;
    http_info_tmp.file_len=0;
    char *p=NULL;
    char *p2=NULL;
    int res=0;
    ///for(){process   out/r/n}//get ,post
    if(in_file_len)
    {
        snprintf(http_info_tmp.filename,512,"%s",filename);
        p=(char *)memmem( in_file_content,in_file_len,"HTTP/1.1",8);
        if(p)
        {
            ///YS 找到HTTP头了
            p=(char *)memmem( in_file_content,in_file_len,"\x0D\x0A\x0D\x0A",4 );
            if(p)
            {
                http_info_tmp.http_hl=(int)(p-in_file_content)+4;
                char *header_buff=NULL;
                header_buff=malloc (sizeof(char)*http_info_tmp.http_hl);
                memmove(header_buff,in_file_content,http_info_tmp.http_hl);
                res=process_http_header(header_buff, &http_info_tmp,1,ip);///YS 处理HTTP头

                ///if(有用的话写文件)
                int len=0;
                if(res==200 || res==206)///YS 304那些都是没有内容的，只有200跟206才会走进来
                {
                    //dbug("get_http_download_filename_and_offset   start !!!!\n");
                    get_http_download_filename_and_offset(header_buff, http_info_tmp.http_hl,&http_info_tmp);///YS 看看有没有偏移量
                    if(http_info_tmp.file_offset)
                    {
                        //        dbug("7777777  have offset ,offset is: %d\n",http_info_tmp.file_offset);
                        len=in_file_len-http_info_tmp.http_hl;
                        if(len>0)
                        {
                            //                dbug("len is %d    name is %s    filelen is %d\n",len,http_info_tmp.filename,http_info_tmp.file_len);
                            ///YS 创建文件，带偏移量的
                            write_file_new(http_info_tmp.filename,len,http_info_tmp.file_offset,in_file_content+http_info_tmp.http_hl,http_info_tmp.file_len,ip,1);
                        }
                        //        dbug("7777777  end!!!1\n");
                        if(len+http_info_tmp.http_hl<in_file_len)
                        {
                            // dbug("memset file ready!\n");
                            memset(in_file_content,0,len+http_info_tmp.http_hl);
                            // dbug("memset file success!\n");
                        }
                        p2=NULL;
                        p=NULL;
                    }
                    else///YS 创建文件，不带偏移量的
                    {
                        ///YS write file until “HTTP/1.1”

                        p2=(char *)memmem( in_file_content+http_info_tmp.http_hl,in_file_len-http_info_tmp.http_hl,"HTTP/1.1",8);
                        if(p2)
                        {
                            len = p2-p-4; ///GET THE LEN
                            //       dbug(" 9999999999 %s  len is %d   %d\n",http_info_tmp.filename,len,http_info_tmp.http_hl);
                            if(len>0)write_file_new(http_info_tmp.filename,len,0,in_file_content+http_info_tmp.http_hl,http_info_tmp.file_len,ip,1);
                            if(len+http_info_tmp.http_hl<in_file_len)
                            {
                                memset(in_file_content,0,len+http_info_tmp.http_hl);
                            }
                            p2=NULL;
                            p=NULL;
                            //memset(in_file_content,0,temp_len);
                        }
                        else
                        {
                            p2=(char *)memmem(in_file_content,in_file_len,"--------",8);///HTTP 特征，一时忘记了，后补
                            if(p2)
                            {
                                len=p2-p+4;
                            }
                            else
                                len=in_file_len-http_info_tmp.http_hl;
                            //          dbug("8888888888 888888888888  %s  len is %d \n",http_info_tmp.filename,len);
                            if(len>0)
                            {
                                //dbug("write file ready!\n");
                                write_file_new(http_info_tmp.filename,len,0,in_file_content+http_info_tmp.http_hl,http_info_tmp.file_len,ip,1);
                                // dbug("write file success!\n");
                            }
                            if(len+http_info_tmp.http_hl<in_file_len)
                            {
                                // dbug("memset file ready!\n");
                                memset(in_file_content,0,len+http_info_tmp.http_hl);
                                // dbug("memset file success!\n");
                            }
                            p2=NULL;
                            p=NULL;
                        }

                    }

                }

                //memset(in_file_content,0,len+http_info_tmp.http_hl);
                if((len>0))
                {
                	  ///YS 清空HTTP头
                    //dbug("move file ready!\n");
                    memmove(in_file_content,in_file_content+len+http_info_tmp.http_hl,in_file_len-len-http_info_tmp.http_hl);
                    memset(in_file_content+in_file_len-len-http_info_tmp.http_hl,'-',len+http_info_tmp.http_hl);
                    //dbug("move file success!\n");
                }
                free(header_buff);
                // free(header_buff);
            }
        }
    }
    return 0;
}
