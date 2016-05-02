/**************************************
这文件的函数是用于审计HTTPBBS的发贴，回贴的。
***************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "time.h"
#include "packet_info.h"
#include "config.h"
#include <mysql.h>

struct bbs_info bbs[Audit_count];
extern int bbs_enable;
extern int code_convert_email(char *keyword);
extern int put_bbs_record_into_db(char *user_ip,char *content,char *host,char *username,char * title,char *url);


/*******************
该函数用于匹配BBS的HOST，每个BBS的HOST在数据库中保存了
参数1，BUFF是要审计的TCP流的内容，这个流一般是POST
参数2，是BUFF的长度
*******************/
int get_bbs_host(char *buff,long buff_len)
{
    int i=0;
    ///获取HOST
    for(i=0; i<Audit_count; i++)
    {
        if(strlen(bbs[i].host)<3) break;
        if(bbs[i].host_url_type==2)///YS 这里要注意了，因为有的BBS的URL是2级域名的
        {
            if(memmem(buff ,buff_len,bbs[i].host_1,strlen(bbs[i].host_1) ))///YS 如果是2级域名的话，需要查到两个关键字，才算找到HOST了
            {
                if(memmem( buff,buff_len,bbs[i].host_2,strlen(bbs[i].host_2) ))
                {
                    return bbs[i].case_value;///YS 这个CASE_VALUE就是这个HOST的ID了
                }
            }
            continue;
        }
        else///YS 不是二级域名的，直接到这，像百度贴吧
        {
            if(memmem( buff,buff_len,bbs[i].host,strlen(bbs[i].host) ))///YS 直接找HOST
            {
                dbug("bssssssssss   %s\n",bbs[i].host);
                return bbs[i].case_value;
            }
            continue;
        }
    }

    return 0;
}



/*******************
该函数用于匹配BBS的HOST，每个BBS的HOST在数据库中保存了
参数1，BUFF是要审计的TCP流的内容，这个流一般是POST
参数2，CASE_NUM是这个流所找到的HOST返回来的CASE_VALUE
参数3，BUFF长度
参数4，用户IP
*******************/
int  get_bbs_info(char *buff,int case_num,long buff_len,char *ip)
{
    char *p=NULL;
    int len_start,len_end;
    char poster[256];
    char title[256];
    char content[1024*100];
    char url[1024*10];
		///get poster
		memset(poster,0,sizeof(poster));
    p=(char *)memmem( buff,buff_len,bbs[case_num].poster_s,strlen(bbs[case_num].poster_s));///YS 找一下有没有发贴人
    if(p)
    {
        len_start=p-buff;
        p=(char *)memmem( buff+len_start+strlen(bbs[case_num].poster_s),buff_len,bbs[case_num].poster_e,strlen(bbs[case_num].poster_e) );
        if(p)///YS 找到发贴人
        {

            len_end=p-buff;
            if(len_end-len_start-strlen(bbs[case_num].poster_s)+1>sizeof(poster))///YS 判断长度是否过长了,结果长度是否大于sizeof(poster)
            {
                snprintf(poster,sizeof(poster),"%s",buff+len_start+strlen(bbs[case_num].poster_s));
            }
            else
            {
                snprintf(poster,len_end-len_start-strlen(bbs[case_num].poster_s)+1,"%s",buff+len_start+strlen(bbs[case_num].poster_s));
            }
        }
    }
    else///YS 因为有的网站发贴人的字段会有多个特征，第一个特征找不到，找第二种情况，下面出现XX2也是如此的情况
    {
        if(strlen(bbs[case_num].poster_s_2))
        {
            p=(char *)memmem( buff,buff_len,bbs[case_num].poster_s_2,strlen(bbs[case_num].poster_s_2));
            if(p)
            {
                len_start=p-buff;
                p=(char *)memmem( buff+len_start+strlen(bbs[case_num].poster_s_2),buff_len,bbs[case_num].poster_e_2,strlen(bbs[case_num].poster_e_2) );
                if(p)
                {
                    len_end=p-buff;
                    if(len_end-len_start-strlen(bbs[case_num].poster_s_2)+1>sizeof(poster))
                    {
                        snprintf(poster,sizeof(poster),"%s",buff+len_start+strlen(bbs[case_num].poster_s_2));
                    }
                    else
                    {
                        snprintf(poster,len_end-len_start-strlen(bbs[case_num].poster_s_2)+1,"%s",buff+len_start+strlen(bbs[case_num].poster_s_2));
                    }
                }
            }
        }
    }


    ///get title 标题
    p=(char *)memmem( buff,buff_len,bbs[case_num].title_s,strlen(bbs[case_num].title_s) );
    if(p)
    {
        memset(title,0,sizeof(title));
        len_start=p-buff;
        p=(char *)memmem( buff+len_start+strlen(bbs[case_num].title_s),buff_len,bbs[case_num].title_e,strlen(bbs[case_num].title_e) );
        if(p)///YS 找到发贴的标题
        {

            len_end=p-buff;
            if(len_end-len_start-strlen(bbs[case_num].title_s)+1>sizeof(title))
            {
                snprintf(title,sizeof(title),"%s",buff+len_start+strlen(bbs[case_num].title_s));
            }
            else
            {
                snprintf(title,len_end-len_start-strlen(bbs[case_num].title_s)+1,"%s",buff+len_start+strlen(bbs[case_num].title_s));
            }
        }
    }
    else///YS 第一个特征找不到，找第二种情况
    {
        if(strlen(bbs[case_num].title_s_2))
        {
            p=(char *)memmem( buff,buff_len,bbs[case_num].title_s_2,strlen(bbs[case_num].title_s_2));
            if(p)
            {
                len_start=p-buff;
                p=(char *)memmem( buff+len_start+strlen(bbs[case_num].title_s_2),buff_len,bbs[case_num].title_e_2,strlen(bbs[case_num].title_e_2) );
                if(p)
                {
                    len_end=p-buff;
                    if(len_end-len_start-strlen(bbs[case_num].title_s_2)+1>sizeof(title))
                    {
                        snprintf(title,sizeof(title),"%s",buff+len_start+strlen(bbs[case_num].title_s_2));
                    }
                    else
                    {
                        snprintf(title,len_end-len_start-strlen(bbs[case_num].title_s_2)+1,"%s",buff+len_start+strlen(bbs[case_num].title_s_2));
                    }
                }
            }
        }
    }

    ///get url
    p=(char *)memmem( buff,buff_len,bbs[case_num].post_url_feature_s,strlen(bbs[case_num].post_url_feature_s) );
    if(p)
    {

        len_start=p-buff;
        p=(char *)memmem( buff+len_start+strlen(bbs[case_num].post_url_feature_s),buff_len,bbs[case_num].post_url_feature_e,strlen(bbs[case_num].post_url_feature_e) );
        if(p)///YS 找一下发贴URL
        {
            len_end=p-buff;
            if(len_end-len_start-strlen(bbs[case_num].post_url_feature_s)+1>sizeof(url))
            {
                snprintf(url,sizeof(url),"%s",buff+len_start+strlen(bbs[case_num].post_url_feature_s));
            }
            else
            {
                snprintf(url,len_end-len_start-strlen(bbs[case_num].post_url_feature_s)+1,"%s",buff+len_start+strlen(bbs[case_num].post_url_feature_s));
            }
        }
    }
    else///YS 第一个特征找不到，找第二种情况
    {
        if(strlen(bbs[case_num].post_url_feature_s_2))
        {
            p=(char *)memmem( buff,buff_len,bbs[case_num].post_url_feature_s_2,strlen(bbs[case_num].post_url_feature_s_2));
            if(p)
            {
                len_start=p-buff;
                p=(char *)memmem( buff+len_start+strlen(bbs[case_num].post_url_feature_s_2),buff_len,bbs[case_num].post_url_feature_e_2,strlen(bbs[case_num].post_url_feature_e_2) );
                if(p)
                {
                    len_end=p-buff;
                    if(len_end-len_start-strlen(bbs[case_num].post_url_feature_s_2)+1>sizeof(url))
                    {
                        snprintf(url,sizeof(url),"%s",buff+len_start+strlen(bbs[case_num].post_url_feature_s_2));
                    }
                    else
                    {
                        snprintf(url,len_end-len_start-strlen(bbs[case_num].post_url_feature_s_2)+1,"%s",buff+len_start+strlen(bbs[case_num].post_url_feature_s_2));
                    }
                }
            }
        }
    }


    ///get content
    p=(char *)memmem( buff,buff_len,bbs[case_num].content_feature_s,strlen(bbs[case_num].content_feature_s) );
    if(p)
    {
        len_start=p-buff;
        p=(char *)memmem( buff+len_start+strlen(bbs[case_num].content_feature_s),buff_len,bbs[case_num].content_feature_e,strlen(bbs[case_num].content_feature_e) );
        if(p)///YS 找到发贴的内容
        {

            len_end=p-buff;
            if(len_end-len_start-strlen(bbs[case_num].content_feature_s)+1>sizeof(content))
            {
                snprintf(content,sizeof(content),"%s",buff+len_start+strlen(bbs[case_num].content_feature_s));
            }
            else
            {
                snprintf(content,len_end-len_start-strlen(bbs[case_num].content_feature_s)+1,"%s",buff+len_start+strlen(bbs[case_num].content_feature_s));
            }
        }
    }
    else///YS 第一个特征找不到，找第二种情况
    {
        if(strlen(bbs[case_num].content_feature_s_2))
        {
            p=(char *)memmem( buff,buff_len,bbs[case_num].content_feature_s_2,strlen(bbs[case_num].content_feature_s_2));
            if(p)
            {
                len_start=p-buff;
                p=(char *)memmem( buff+len_start+strlen(bbs[case_num].content_feature_s_2),buff_len,bbs[case_num].content_feature_e_2,strlen(bbs[case_num].content_feature_e_2) );
                if(p)
                {
                    len_end=p-buff;
                    if(len_end-len_start-strlen(bbs[case_num].content_feature_s_2)+1>sizeof(content))
                    {
                        snprintf(content,sizeof(content),"%s",buff+len_start+strlen(bbs[case_num].content_feature_s_2));
                    }
                    else
                    {
                        snprintf(content,len_end-len_start-strlen(bbs[case_num].content_feature_s_2)+1,"%s",buff+len_start+strlen(bbs[case_num].content_feature_s_2));
                    }
                }
            }
        }
    }


if(strlen(poster)<2)///YS 有一些BBS网站没有发贴的帐号，这时我们用IP，谁，发了这个贴
{
    sprintf(poster,"%s","NULL");
}

    if(strlen(content)&&strlen(poster)>3 )
    {
//        code_convert_email(poster);///YS 以经不需要转了，让PHP处理去
  //      code_convert_email(content);


        dbug("#####post  title   %s  \n",title);
        dbug("#####post  content   %s  \n",content);
        dbug("#####post  poster   %s  \n",poster);
        dbug("#####post  url   %s  \n",url);
        if(strlen(title)>5)///YS 如果有标题的话，就写数据库
        {
            //code_convert_email(title);
            put_bbs_record_into_db(ip,content,bbs[case_num].host,poster,title,url);
        }
        else///YS 快速回复的话，不一定有标题喔
        {
            sprintf(title,"null");
            put_bbs_record_into_db(ip,content,bbs[case_num].host,poster,title,url);
        }
    }
    return 0;
}



/*******************
BBS的入口
参数1，BUFF是要审计的TCP流的内容，这个流一般是POST
参数2，是BUFF的长度
参数3，用户IP
*******************/
int process_bbs(char  * buff,long buff_len,char *ip)
{
    //   if(s->already_process==1) return;
//ys  post ---> email  ys//
    if(!bbs_enable) return 0;
    int case_num =get_bbs_host(buff , buff_len);
    if(case_num)
    {
        get_bbs_info(buff,case_num-1,buff_len,ip);
    }
    else
    {
//        dbug("===============  cant find  bbs host ++++++++++   \n");
    }

    return 0;
}
