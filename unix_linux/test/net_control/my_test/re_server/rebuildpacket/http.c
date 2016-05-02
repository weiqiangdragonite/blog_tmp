///YS 注意：该文件的名字叫HTTP.C实际上这个跟HTTP没关系，
///YS 本来是打算写HTTP相关的函数的，后来将错就错了，
///YS 包括process_out_stream这个函数，也不仅仅是只给OUT这个方向的流用的

#include <stdio.h>
#include "time.h"
#include "packet_info.h"
#include "config.h"
#include <mysql.h>
#include <string.h>

extern int http_enable;
extern char *get_file_content (char *filename,long *len,char **buffer);
extern int process_email(char * buff,int buff_len,char *user_ip,int post_or_get);
extern int  process_http_header(const char *header_buff,   struct http_info * http_info_tmp,int direct ,char *ip);
extern int upload_file_name(char *buff,int buff_len,char *ip);
extern int process_im( char * buff,long int buff_len);
extern int process_in_stream(const char * in_file_content,char  *filename,long in_file_len,char *ip);
extern int process_post_boundary(const char * out_file_content, long out_file_len,char *ip);
extern int http_search(char * s,int len,char *user_ip);
extern int process_bbs(char  * buff,long buff_len,char *ip);
extern int process_email_post(char * buff,int buff_len,char *user_ip,int SMTP_POP3);
extern int process_email_get(char * buff,int buff_len);
extern int read_email_html(char * buff,char *user_ip,int buff_len,int case_num);




/****************************
处理我们的TCP流，包括IN,OUT两个方向的流
参数1，IN—STREAM的字符，如192.168.1.12999221.23.11.2280_in
参数2，OUT-STREAM的字符，如192.168.1.12999221.23.11.2280_out
参数3，用户的IP
****************************/
int process_out_stream(char *in_file,char *out_file,char *ip)
{
    int i=0,ret=0;
    struct http_info http_info_tmp;
    char *p=NULL,*p2=NULL;
    char *out_file_content=NULL;
    char *in_file_content=NULL;
    long in_file_len=0;
    long out_file_len=0;
    ///for(){process   out/r/n}//get ,post
    get_file_content (out_file,&out_file_len,&out_file_content);
    get_file_content (in_file,&in_file_len,&in_file_content);

		///YS  提前处理我们的内容
 	process_email_post(out_file_content,out_file_len,ip,0);///YS 处理EMAIL，处理的是POST的，即发邮件的
    http_search(out_file_content,out_file_len,ip);///YS 处理搜索
    process_bbs(out_file_content,out_file_len,ip);///YS 处理BBS
    if(out_file_len)
    {
        for(i=0; i<100; i++)///YS 一个流里面可能会有多个HTTP，如KEEPALIVE，这个FOR语句仅针对HTTP
        {
            p=(char *)memmem( out_file_content,out_file_len,"GET ",4);
            if(p)
            {
                ///YS 找到HTTP头了
                p=(char *)memmem( out_file_content,out_file_len,"\x0D\x0A\x0D\x0A",4 );
                if(p)
                {
                    http_info_tmp.http_hl=(int)(p-out_file_content)+4;
                    char *header_buff=  (char*) malloc (sizeof(char)*http_info_tmp.http_hl);
                    memmove(header_buff,out_file_content,http_info_tmp.http_hl);///YS 处理完的信息可以不要了，这里使用MOVE直接覆盖掉，下面的情况也是
                    process_http_header(header_buff, &http_info_tmp,2,ip);///YS 处理HTTP头
                    if((ret=process_email_get(header_buff,http_info_tmp.http_hl)))///YS 处理EMAIL，GET，即收邮件
                    {
                        read_email_html(in_file_content,ip,in_file_len,ret-1);

                    }
                    ///YS 清空HTTP头
                    memset(out_file_content,0,http_info_tmp.http_hl);
                    memmove(out_file_content,out_file_content+http_info_tmp.http_hl,out_file_len-http_info_tmp.http_hl);
                    memset(out_file_content+out_file_len-http_info_tmp.http_hl,0,http_info_tmp.http_hl);

                    free(header_buff);
                    ///YS 处理完GET，对应的HTTP 200，304也要处理
                    if(in_file_len>0)
                    {
												///YS 这里的INstream,是对应用的GET，返回来的内容
                        process_in_stream(in_file_content,http_info_tmp.filename,in_file_len,ip);
                    }
                    continue;
                }
            }
            else///YS 上面是处理GET的，下面是处理POST
            {
						//dbug("for POST !!!!\n");

                p=(char *)memmem( out_file_content,out_file_len,"POST ",5 );
                if(p)
                {
                    p=(char *)memmem( out_file_content,out_file_len,"\x48\x54\x54\x50\x2F\x31\x2E\x31\x0D\x0A",10 ); ///YS 同样的，POST也会有KEEPLIVE情况，一个流里存在多个HTTP头，那一串16进制是HTTP/1.1\R\N
                    if(p)///YS 找到HTTP头，POST的头
                    {
                        process_email_post(out_file_content,out_file_len,ip,0);///处理EMAIL POST，发邮件不过前面好像已经处理过了，有代测试
                        p2=(char *)memmem( p+10,out_file_len,"\x48\x54\x54\x50\x2F\x31\x2E\x31\x0D\x0A",10 ); //HTTP/1.1\R\N
                        if(p2)///YS 找到第二个HTTP头
                        {
                            dbug("goto  upload_file_name  function %ld\n",p2-p);
                            ///YS 确定写入的长度，到第二个HTTP1.1前，用于分割多个HTTP信息
                            int len=p2-p;
                            upload_file_name(out_file_content,len,ip);///YS 这个方法是用正则式的，通用性强，不过文件名，跟长度都是有一定的误差，不过可以接受。下面这个注释了的代码是可以运行的，是针对特定的上传方式boundary才用的。
                            memset(out_file_content,0,len);
                            memmove(out_file_content,out_file_content+len,out_file_len-len);
                            memset(out_file_content+out_file_len-len,0,len);
                        }
                        else
                        {
                            ///YS 没有找到第二个HTTP头
                            upload_file_name(out_file_content,out_file_len,ip);///这个方法是用正则式的，通用性强，不过文件名，跟长度都是有一定的误差，不过可以接受。下面这个注释了的代码是可以运行的，是针对特定的上传方式boundary才用的。
                            memset(out_file_content,0,out_file_len);
                        }


                    }

                    /*
                    p=(char *)memmem( out_file_content,out_file_len,"\x0D\x0A\x0D\x0A",4 );///找到HTTP头了
                    if(p)
                    {
                        http_info_tmp.http_hl=(int)(p-out_file_content)+4;
                        char *header_buff=  (char*) malloc (sizeof(char)*http_info_tmp.http_hl);
                        memmove(header_buff,out_file_content,http_info_tmp.http_hl);
                        ret=process_http_header(header_buff, &http_info_tmp,3);
                        ///清空HTTP头
                        memset(out_file_content,0,http_info_tmp.http_hl);
                        memmove(out_file_content,out_file_content+http_info_tmp.http_hl,out_file_len-http_info_tmp.http_hl);
                        memset(out_file_content+out_file_len-http_info_tmp.http_hl,0,http_info_tmp.http_hl);
                        free(header_buff);
                        if(ret==3)///boundary
                        {
                            process_post_boundary(out_file_content,out_file_len,ip);
                        }*/


                    continue;
                }
                else
                {
                    break;
                }
            }
        }
    }
    free(out_file_content);
    free(in_file_content);
    in_file_content=NULL;
    out_file_content=NULL;
    return 0;
}

