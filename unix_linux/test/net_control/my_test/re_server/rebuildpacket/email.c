///YS 获取EMAIL审计的内容的函数都在这个文件
#include <stdio.h>
#include "time.h"
#include "packet_info.h"
#include "config.h"
#include <mysql/mysql.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>

extern struct email_info email[Audit_count];
extern struct email_html_info email_html[Audit_count];
extern int email_enable;
extern int httpgzdecompress(Byte *zdata, uLong nzdata, uLong *ndata);
extern int UnzipHttpFile(char *s,int iSLen,unsigned long *knlen/*,Byte *d,unsigned long * iDLen*/);
int put_email_record_into_db(char *user_ip,char *content,char *host,char *s_email,char * r_email,char *title);
extern char date[24];
extern int put_email_read_into_db(char *user_ip,char *filename);
extern char zip_destbuff[1024*1024*2];


int process_pop3(char *buff,char *user_ip,long buff_len,int case_num)
{
    char *p;
    long len_start,len_end;
    char sendmailname[256]= {0};
    char receiver[256]= {0};
    char title[256]= {0};
    char content[1024*100]= {0};

    p=(char *)memmem( buff,buff_len,email_html[case_num].sender_s,strlen(email_html[case_num].sender_s));///YS 获取发送人的EMAIL
    if(p)
    {

        len_start=p-buff;
        p=(char *)memmem( buff+len_start+strlen(email_html[case_num].sender_s),buff_len,email_html[case_num].sender_e,strlen(email_html[case_num].sender_e) );
        if(p)///YS 找到发送人的EMAIL
        {
            len_end=p-buff;

            if(len_end-len_start-strlen(email_html[case_num].sender_s)+1>sizeof(sendmailname))///YS 判断长度是否比我们的准备的字符数组要大
            {
                snprintf(sendmailname,sizeof(sendmailname),"%s",buff+len_start+strlen(email_html[case_num].sender_s));
            }
            else
            {
                snprintf(sendmailname,len_end-len_start-strlen(email_html[case_num].sender_s)+1,"%s",buff+len_start+strlen(email_html[case_num].sender_s));
            }
        }
    }

    p=(char *)memmem( buff,buff_len,email_html[case_num].receiver_s,strlen(email_html[case_num].receiver_s));///YS 或取收件人EMAIL
    if(p)
    {
        len_start=p-buff;
        p=(char *)memmem( buff+len_start+strlen(email_html[case_num].receiver_s),buff_len,email_html[case_num].receiver_e,strlen(email_html[case_num].receiver_e) );
        if(p)///YS 找到有收件人
        {
            len_end=p-buff;
            if(len_end-len_start-strlen(email_html[case_num].receiver_s)+1>sizeof(receiver))///YS 判断长度是否过长
            {
                snprintf(receiver,sizeof(receiver),"%s",buff+len_start+strlen(email_html[case_num].receiver_s));
            }
            else
            {
                snprintf(receiver,len_end-len_start-strlen(email_html[case_num].receiver_s)+1,"%s",buff+len_start+strlen(email_html[case_num].receiver_s));
            }
        }
    }

    p=(char *)memmem( buff,buff_len,email_html[case_num].title_s,strlen(email_html[case_num].title_s));///YS 获取发邮件的标题
    if(p)
    {
        len_start=p-buff;

        p=(char *)memmem( buff+len_start+strlen(email_html[case_num].title_s),buff_len,email_html[case_num].title_e,strlen(email_html[case_num].title_e) );
        if(p)///YS 找到发邮件的标题
        {
            len_end=p-buff;

            if(len_end-len_start-strlen(email_html[case_num].title_s)+1>sizeof(title))///YS 判断长度
            {
                snprintf(title,sizeof(title),"%s",buff+len_start);
            }
            else
            {
                snprintf(title,len_end-len_start+strlen(email_html[case_num].title_e)+1,"%s",buff+len_start);
            }
        }
        else
        {
            p=(char *)memmem( buff+len_start+strlen(email_html[case_num].title_s),buff_len,email_html[case_num].title_e2,strlen(email_html[case_num].title_e2) );
            if(p)///YS 找到发邮件的标题
            {
                len_end=p-buff;

                if(len_end-len_start-strlen(email_html[case_num].title_s)+1>sizeof(title))///YS 判断长度
                {
                    snprintf(title,sizeof(title),"%s",buff+len_start);
                }
                else
                {
                    snprintf(title,len_end-len_start+strlen(email_html[case_num].title_e2)+1,"%s",buff+len_start);
                }
            }
        }
    }


    p=(char *)memmem( buff,buff_len,email_html[case_num].content_s,strlen(email_html[case_num].content_s));///YS 获取内容
    if(p)
    {
        len_start=p-buff;
        p=(char *)memmem( buff+len_start+strlen(email_html[case_num].content_s),buff_len,email_html[case_num].content_e,strlen(email_html[case_num].content_e) );
        if(p)///YS 找到内容
        {
            len_end=p-buff;
            if(len_end-len_start-strlen(email_html[case_num].content_s)+1>sizeof(content))///YS 判断长度
            {
                snprintf(content,sizeof(content),"%s",buff+len_start);
            }
            else
            {
                snprintf(content,len_end-len_start-strlen(email_html[case_num].content_s)+1,"%s",buff+len_start);
            }
        }
    }

    if(strlen(content)>3 && strlen(receiver)>3&& strlen(sendmailname) >3&&strlen(title)>3 )///YS 如果发件人，内容，收件人，标题都拿到了，我们才写进数据库
    {
       // dbug("8888888888888888888888   %s   %s   %s    %s\n",(sendmailname),receiver,content,title);
        put_email_POP3_into_db(user_ip,content,title,sendmailname,receiver);
    }

    return 0;
}

int get_pop3_content(char * buff,int buff_len,char * user_ip,int case_num)
{
    long len_start,len_end,content_len;
    char *p;

    char email_content[1024*1024]= {0};
    while(1)
    {
        p=(char *)memmem( buff,buff_len,"From: ",6);///YS 获取发送人的EMAIL
        if(p)
        {
            len_start=p-buff;
            p=(char *)memmem( buff+len_start,buff_len-4,"\x0D\x0A\x0D\x0A\x2E\x0D\x0A", 7);
            if(p)///YS 找到发送人的EMAIL
            {
                len_end=p-buff;
                content_len=len_end-len_start+7;

                dbug("aaaaaaaaaaaaaaaaaa11111111111  %d\n",content_len);
                memmove(email_content,buff+len_start,content_len);
                process_pop3(email_content,user_ip,content_len,case_num);
                memset(buff+len_start,0,content_len);
                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    return 0;
}

int pop3(char * in_file,char *user_ip)
{

    char *in_file_content=NULL;
    long in_file_len=0;
    int ret;
    get_file_content (in_file,&in_file_len,&in_file_content);

    ret=get_email_host_for_readmail(in_file_content,in_file_len);
    if(ret)
    {
        get_pop3_content(in_file_content,in_file_len,user_ip,ret-1);

    }

    return 0;
}


int smtp(char * out_file,char * in_file,char *user_ip)
{
    char *out_file_content=NULL;
    long out_file_len=0;
    char *in_file_content=NULL;
    long in_file_len=0;
    int case_num;
    get_file_content (out_file,&out_file_len,&out_file_content);

    get_file_content (in_file,&in_file_len,&in_file_content);

    case_num=process_email_post(in_file_content,in_file_len,user_ip,1);///YS 处理EMAIL，处理的是POST的，即发邮件的
    if(case_num)
    {
        (get_email_info(out_file_content,case_num-1,user_ip,out_file_len));///YS 注意这里的CASE_NUM-1，之所以要减1，是因为要与数据库里的CASE_VALUE字段对齐

    }
    free(out_file_content);
    free(in_file_content);
    in_file_content=NULL;
    out_file_content=NULL;
    return 0;
}

/***********************
邮件阅读的审计功能
参数1，BUFF是TCP重组后的BUFF，仅可以是一个方向的BUFF，要么是出的方向的BUFF，要么是进的，因为是读EMAIL，以HTTPGET为主，所以方向应该为TCP流里的IN方向
参数2，用户的IP
参数3，传进来的BUFF的长度
参数4，CASE_NUM是相应的要审计的具体邮件网址，如1是QQMAIL,2是163MAIL,3是GMAIL等
***********************/
int read_email_html(char * buff,char *user_ip,int buff_len,int case_num)
{
    char *p=NULL;
    int len_start;
    unsigned long html_len;
    char filename[1024];
    char  dir[1024];
    if(buff_len<30) return 0;
    p=(char *)memmem( buff+3,buff_len-3,"HTTP/1.1",8);
    if(p)
    {
        buff_len=p-buff;
    }



    p=(char *)memmem( buff,buff_len,email_html[case_num].get_feature,strlen(email_html[case_num].get_feature));///YS 是否有GET的标志
    if(p)
    {
        p=(char *)memmem( buff,buff_len,email_html[case_num].http_respont_feature,strlen(email_html[case_num].http_respont_feature));///YS 是否有HTTP返回的标志
        if(p)
        {
            p=(char *)memmem( buff,buff_len,"\x0D\x0A\x0D\x0A",4 );///YS 找到HTTP头的结尾
            if(p)
            {
                snprintf(filename,sizeof(filename),tmp_email_file_path,email_html[case_num].host,user_ip,date,rand());///YS 用随机数来生成一个文件名 ，如32123142.HTML
                snprintf(dir,sizeof(dir),tmp_email_file_dir,email_html[case_num].host,user_ip,date);

                if(access(dir,0)<0)
                {
                    char mkdir_str[64];
                    sprintf(mkdir_str,"mkdir -p %s",dir);
                    system(mkdir_str);
                    dbug("dir not exiest \n");
                }
                len_start=p-buff+4;
                if(buff_len-len_start<10)return 0;
                dbug("email read>>>>>>>>>>>>> filename  len?  %d   %s\n",buff_len-len_start,filename);


                int ret;
                p=(char *)memmem( buff,buff_len,"gzip",4 );///YS 看看是不是GZIP压缩了
                if(p)
                {
                    dbug("email read>>>>>>>>>>>>> has gzip\n");
                    p=(char *)memmem( buff,buff_len,"chunked",7 );
                    if(p)
                        ret=httpgzdecompress(buff+len_start+5,buff_len-len_start-5,&html_len);///YS 进行GZIP解压
                    else  ret=httpgzdecompress(buff+len_start,buff_len-len_start,&html_len);
                    if(ret==0 && html_len >0)
                    {
                        if(!(memmem(zip_destbuff,html_len,email_html[case_num].save_flag_1,strlen(email_html[case_num].save_flag_1)))) return 0;
                        dbug("email read>>>>>>>>>>>>> create zip file\n");
                        FILE *stream;
                        stream = fopen(filename,"w+");
                        fwrite(zip_destbuff,html_len,1,stream);///YS 写入文件
                        fclose(stream);
                        put_email_read_into_db(user_ip,filename);
                    }
                }
                else///YS 不是GZIP压缩的，那就直接是明文，写入文件就行
                {
                    if(!(memmem(zip_destbuff,html_len,email_html[case_num].save_flag_1,strlen(email_html[case_num].save_flag_1)))) return 0;
                    dbug("email read>>>>>>>>>>>>> create Unzip html 000\n");

                    if(buff_len-len_start >0)
                    {
                        FILE *stream;
                        stream = fopen(filename,"w+");
                        fwrite(buff+len_start,buff_len-len_start,1,stream);
                        fclose(stream);
                        put_email_read_into_db(user_ip,filename);
                    }


                }
            }
        }
    }
    return 0;
}

/*****************************
获取EMAIL的host（是163，还是QQ，还是GMAIL），并返回相应的CASE_NUM
*****************************/
int get_email_host(char * buff,int buff_len)//1post,0get
{
    int i=0;
    ///获取HOST

    for(i=0; i<Audit_count; i++)
    {
        if(strlen(email[i].host))
        {
            if(memmem( buff,buff_len,email[i].host,strlen(email[i].host)))
            {
                dbug("get the email host   %s\n",email[i].host);
                return email[i].case_value;
            }
        }
        else
        {
            return 0;
        }
    }

    return 0;
}

/*****************************
获取EMAIL的host（是163，还是QQ，还是GMAIL），并返回相应的CASE_NUM
*****************************/
int get_email_host_for_readmail(char * buff,int buff_len)//1post,0get
{
    int i=0;
    ///获取HOST

    for(i=0; i<Audit_count; i++)
    {
        if(strlen(email_html[i].host))
        {
            if(memmem( buff,buff_len,email_html[i].host,strlen(email_html[i].host)))
            {
                dbug("get the email host   %s\n",email_html[i].host);
                return email_html[i].case_value;
            }
        }
        else
        {
            return 0;
        }
    }

    return 0;
}


/***********************
获取EMAIL内容
参数1，BUFF是TCP流
参数2，是哪一个EMAIL（是163,还是QQ...)
参数3，是用户IP
参数4，BUFF长度
**********************/

int get_email_info(char * buff,int case_num,char *user_ip,int buff_len)
{
    char *p=NULL;
    int len_start,len_end;
    char sendmailname[256]= {0};
    char receiver[256];
    char title[256];
    char content[1024*100];
    p=(char *)memmem( buff,buff_len,email[case_num].sender_s,strlen(email[case_num].sender_s));///YS 获取发送人的EMAIL
    if(p)
    {
        len_start=p-buff;
        p=(char *)memmem( buff+len_start+strlen(email[case_num].sender_s),buff_len,email[case_num].sender_e,strlen(email[case_num].sender_e) );
        if(p)///YS 找到发送人的EMAIL
        {
            len_end=p-buff;

            if(len_end-len_start-strlen(email[case_num].sender_s)+1>sizeof(sendmailname))///YS 判断长度是否比我们的准备的字符数组要大
            {
                snprintf(sendmailname,sizeof(sendmailname),"%s",buff+len_start+strlen(email[case_num].sender_s));
            }
            else
            {
                snprintf(sendmailname,len_end-len_start-strlen(email[case_num].sender_s)+1,"%s",buff+len_start+strlen(email[case_num].sender_s));
            }
        }
    }
    else
    {
        if(strlen(email[case_num].sender_s_2))///YS 有的邮件的发件EMAIL会以多种情况出现
        {
            p=(char *)memmem( buff,buff_len,email[case_num].sender_s_2,strlen(email[case_num].sender_s_2));
            if(p)
            {
                len_start=p-buff;
                p=(char *)memmem( buff+len_start+strlen(email[case_num].sender_s_2),buff_len,email[case_num].sender_e_2,strlen(email[case_num].sender_e_2) );
                if(p)
                {
                    len_end=p-buff;

                    if(len_end-len_start-strlen(email[case_num].sender_s_2)+1>sizeof(sendmailname))
                    {
                        snprintf(sendmailname,sizeof(sendmailname),"%s",buff+len_start+strlen(email[case_num].sender_s_2));
                    }
                    else
                    {
                        snprintf(sendmailname,len_end-len_start-strlen(email[case_num].sender_s_2)+1,"%s",buff+len_start+strlen(email[case_num].sender_s_2));
                    }

                }
            }
        }
    }



    p=(char *)memmem( buff,buff_len,email[case_num].receiver_s,strlen(email[case_num].receiver_s));///YS 或取收件人EMAIL
    if(p)
    {
        len_start=p-buff;
        p=(char *)memmem( buff+len_start+strlen(email[case_num].receiver_s),buff_len,email[case_num].receiver_e,strlen(email[case_num].receiver_e) );
        if(p)///YS 找到有收件人
        {
            len_end=p-buff;
            if(len_end-len_start-strlen(email[case_num].receiver_s)+1>sizeof(receiver))///YS 判断长度是否过长
            {
                snprintf(receiver,sizeof(receiver),"%s",buff+len_start+strlen(email[case_num].receiver_s));
            }
            else
            {
                snprintf(receiver,len_end-len_start-strlen(email[case_num].receiver_s)+1,"%s",buff+len_start+strlen(email[case_num].receiver_s));
            }
        }
    }
    else
    {
        if(strlen(email[case_num].receiver_s_2))///YS 收件人是否以第二种情况的特征出现
        {
            p=(char *)memmem( buff,buff_len,email[case_num].receiver_s_2,strlen(email[case_num].receiver_s_2));
            if(p)
            {
                len_start=p-buff;
                p=(char *)memmem( buff+len_start+strlen(email[case_num].receiver_s_2),buff_len,email[case_num].receiver_e_2,strlen(email[case_num].receiver_e_2) );
                if(p)
                {
                    len_end=p-buff;

                    if(len_end-len_start-strlen(email[case_num].receiver_s_2)+1>sizeof(receiver))
                    {
                        snprintf(receiver,sizeof(receiver),"%s",buff+len_start+strlen(email[case_num].receiver_s_2));
                    }
                    else
                    {
                        snprintf(receiver,len_end-len_start-strlen(email[case_num].receiver_s_2)+1,"%s",buff+len_start+strlen(email[case_num].receiver_s_2));
                    }

                }
            }
        }
    }

    p=(char *)memmem( buff,buff_len,email[case_num].title_s,strlen(email[case_num].title_s));///YS 获取发邮件的标题
    if(p)
    {
        len_start=p-buff;
        p=(char *)memmem( buff+len_start+strlen(email[case_num].title_s),buff_len,email[case_num].title_e,strlen(email[case_num].title_e) );
        if(p)///YS 找到发邮件的标题
        {
            len_end=p-buff;

            if(len_end-len_start-strlen(email[case_num].title_s)+1>sizeof(title))///YS 判断长度
            {
                snprintf(title,sizeof(title),"%s",buff+len_start+strlen(email[case_num].title_s));
            }
            else
            {
                snprintf(title,len_end-len_start-strlen(email[case_num].title_s)+1,"%s",buff+len_start+strlen(email[case_num].title_s));
            }
        }
    }
    else
    {
        if(strlen(email[case_num].title_s_2))///YS 是否以第二种情况的特征出现
        {
            p=(char *)memmem( buff,buff_len,email[case_num].title_s_2,strlen(email[case_num].title_s_2));
            if(p)
            {
                len_start=p-buff;
                p=(char *)memmem( buff+len_start+strlen(email[case_num].title_s_2),buff_len,email[case_num].title_e_2,strlen(email[case_num].title_e_2) );
                if(p)///YS 找到标题
                {
                    len_end=p-buff;
                    if(len_end-len_start-strlen(email[case_num].title_s_2)+1>sizeof(title))
                    {
                        snprintf(title,sizeof(title),"%s",buff+len_start+strlen(email[case_num].title_s_2));
                    }
                    else
                    {
                        snprintf(title,len_end-len_start-strlen(email[case_num].title_s_2)+1,"%s",buff+len_start+strlen(email[case_num].title_s_2));
                    }
                }
            }
        }
    }


    p=(char *)memmem( buff,buff_len,email[case_num].content_feature_s,strlen(email[case_num].content_feature_s));///YS 获取内容
    if(p)
    {
        len_start=p-buff;
        p=(char *)memmem( buff+len_start+strlen(email[case_num].content_feature_s),buff_len,email[case_num].content_feature_e,strlen(email[case_num].content_feature_e) );
        if(p)///YS 找到内容
        {
            len_end=p-buff;
            if(len_end-len_start-strlen(email[case_num].content_feature_s)+1>sizeof(content))///YS 判断长度
            {
                snprintf(content,sizeof(content),"%s",buff+len_start+strlen(email[case_num].content_feature_s));
            }
            else
            {
                snprintf(content,len_end-len_start-strlen(email[case_num].content_feature_s)+1,"%s",buff+len_start+strlen(email[case_num].content_feature_s));
            }
        }
    }
    else
    {
        if(strlen(email[case_num].content_feature_s_2))///YS 邮件内容是否以第二种情况出现
        {
            p=(char *)memmem( buff,buff_len,email[case_num].content_feature_s_2,strlen(email[case_num].content_feature_s_2));
            if(p)
            {
                len_start=p-buff;
                p=(char *)memmem( buff+len_start+strlen(email[case_num].content_feature_s_2),buff_len,email[case_num].content_feature_e_2,strlen(email[case_num].content_feature_e_2) );
                if(p)
                {
                    len_end=p-buff;
                    snprintf(content,len_end-len_start-strlen(email[case_num].content_feature_s_2)+1,"%s",buff+len_start+strlen(email[case_num].content_feature_s_2));

                    if(len_end-len_start-strlen(email[case_num].content_feature_s)+1>sizeof(content))
                    {
                        snprintf(content,sizeof(content),"%s",buff+len_start+strlen(email[case_num].content_feature_s_2));
                    }
                    else
                    {
                        snprintf(content,len_end-len_start-strlen(email[case_num].content_feature_s_2)+1,"%s",buff+len_start+strlen(email[case_num].content_feature_s_2));
                    }
                }
            }
        }
    }

    if(strlen(content)>3 && strlen(receiver)>3&& strlen(sendmailname) >3&&strlen(title)>3 )///YS 如果发件人，内容，收件人，标题都拿到了，我们才写进数据库
    {
        dbug("8888888888888888888888    %lu   %s   %s\n",strlen(sendmailname),sendmailname,content);
        put_email_record_into_db(user_ip,content,email[case_num].host,sendmailname,receiver,title);///YS 审计结果写入数据库
        return 1;
    }
    return 0;
}

/****************
发邮件的话，用这个函数，通常用于POST
参数1，BUFF是TCP流里的OUT_STREAM
参数2，是BUFF的长度
参数3，是用户的IP
参数4，if the email use SMTP OR POP3,1for SMTP ; 2 for pop3
****************/
int process_email_post(char * buff,int buff_len,char *user_ip,int SMTP_POP3 )
{
    int case_num =0;
    if(!email_enable) return 0 ;
    if(SMTP_POP3==1)///YS SMTP
    {
        case_num =get_email_host(buff,buff_len);///YS 找一下有没有我们要审计的HOST
        return  case_num;
    }


    case_num =get_email_host(buff,buff_len);///YS 找一下有没有我们要审计的HOST
    if(case_num)
    {
        dbug("1return %d\n",case_num);
        if(get_email_info(buff,case_num-1,user_ip,buff_len))///YS 注意这里的CASE_NUM-1，之所以要减1，是因为要与数据库里的CASE_VALUE字段对齐
        {
            // dbug("2return %d\n",case_num);
            return 0;
        }
    }
    return 0;
}

/****************
读邮件的话，用这个函数，通常用于GET
参数1，BUFF是TCP流里的IN_STREAM
参数2，是BUFF的长度
****************/
int process_email_get(char * buff,int buff_len)
{
    int case_num =0;
    if(!email_enable) return 0 ;


    case_num =get_email_host(buff,buff_len);
    return case_num;

}
