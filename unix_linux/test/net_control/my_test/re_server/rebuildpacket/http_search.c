///YS 搜索审计的函数都在这里
#include <stdio.h>
#include "time.h"
#include "packet_info.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
extern int search_enable;
struct search_info search[Audit_count];
extern int put_search_record_into_db(char *user_ip,char *keyword,char *search_eng);


/*****************
获得搜索的HOST，如baidu,google
参数1，s是我们要处理的TCP流里的其中一个方向
参数2，是这个流的长度
******************/
int get_search_host(char  *s,int len)
{
    int i=0;
    ///获取HOST
    for(i=0; i<Audit_count; i++)
    {
  //dbug("%s  \n",search[i].host);
        if(strlen(search[i].host)<3) break;
        if(memmem( s,len,search[i].host,strlen(search[i].host)))
        {

            if(memmem(s,len,search[i].auto_search,strlen(search[i].auto_search)))
            {
                dbug("auto complete search\n");
                return 0;
            }
            else
            {
                dbug("get thd search  %s\n",search[i].host);
                return search[i].case_value;
            }
        }
    }
    return 0;
}

/*****************
审计搜索的内容
参数1，s是我们要处理的TCP流里的其中一个方向
参数2，这个流所对应的搜索引擎
参数3，用户IP
参数4，是这个流的长度
******************/
int get_search_info(char *s ,int case_num,char *user_ip,int len)
{
    int len_start,len_end;
    char keyword[222];
    memset(keyword,0,222);
    char *p;

    p=(char *)memmem( s,len,search[case_num].key_word_s,strlen(search[case_num].key_word_s));///YS 搜索关键字的开始特征
    if(p)
    {
        len_start=p-s;
        p=(char *)memmem( s+len_start+strlen(search[case_num].key_word_s),len,search[case_num].key_word_e,strlen(search[case_num].key_word_e) );///YS 搜索关键字的结束特征
        if(p)///YS 开始和结束都找到了，关键字找到了
        {
            len_end=p-s;
            if(len_end-len_start-strlen(search[case_num].key_word_s)+1>sizeof(keyword))///YS 判断长度是否过长
            {
                snprintf(keyword,sizeof(keyword),"%s",s+len_start+strlen(search[case_num].key_word_s));
            }
            else
            {
                snprintf(keyword,len_end-len_start-strlen(search[case_num].key_word_s)+1,"%s",s+len_start+strlen(search[case_num].key_word_s));
            }

        }
    }
    else
    {
        if(strlen(search[case_num].key_word_s_2))///YS 是否存在第二种情况
        {
            p=(char *)memmem( s,len,search[case_num].key_word_s_2,strlen(search[case_num].key_word_s_2));
            if(p)
            {
                len_start=p-s;
                p=(char *)memmem( s+len_start+strlen(search[case_num].key_word_s_2),len,search[case_num].key_word_e_2,strlen(search[case_num].key_word_e_2) );
                if(p)
                {
                    len_end=p-s;

                    if(len_end-len_start-strlen(search[case_num].key_word_s_2)+1>sizeof(keyword))
                    {
                        snprintf(keyword,sizeof(keyword),"%s",s+len_start+strlen(search[case_num].key_word_s_2));
                    }
                    else
                    {
                        snprintf(keyword,len_end-len_start-strlen(search[case_num].key_word_s_2)+1,"%s",s+len_start+strlen(search[case_num].key_word_s_2));
                    }


                }
            }
            else
            {
                if(strlen(search[case_num].key_word_s_3))///YS 是否存在第三种情况
                {
                    p=(char *)memmem( s,len,search[case_num].key_word_s_3,strlen(search[case_num].key_word_s_3));
                    if(p)
                    {
                        len_start=p-s;
                        p=(char *)memmem( s+len_start+strlen(search[case_num].key_word_s_3),len,search[case_num].key_word_e_3,strlen(search[case_num].key_word_e_3) );
                        if(p)
                        {
                            len_end=p-s;

                            if(len_end-len_start-strlen(search[case_num].key_word_s_3)+1>sizeof(keyword))
                            {
                                snprintf(keyword,sizeof(keyword),"%s",s+len_start+strlen(search[case_num].key_word_s_3));
                            }
                            else
                            {
                            snprintf(keyword,len_end-len_start-strlen(search[case_num].key_word_s_3)+1,"%s",s+len_start+strlen(search[case_num].key_word_s_3));
                            }

                        }
                    }
                }
            }
        }

    }


    if(strlen(keyword))///YS 如果找到关键字的话
    {
        p=memmem(keyword,222,"HTTP/1.1",8);///YS 如果关键字有HTTP头，则去掉
        if(p)
        {
            len_start=p-keyword;
            memset(keyword+len_start,0,8);
        }
        //dbug("jjjjjjjjjjjjjjjjjjjjjjj   %lu \n",strlen(keyword));
        put_search_record_into_db(user_ip,keyword,search[case_num].host);///YS 插入到数据库
    }
    return 0;
}


/*****************
审计搜索的入口
参数1，s是我们要处理的TCP流里的其中一个方向
参数2，是这个流的长度
参数3，用户IP
******************/
int http_search(char * s,int len,char *user_ip)
{
    if(!search_enable) return 0;
    int case_num =get_search_host(s,len);
    if(case_num)
    {
        get_search_info(s,case_num-1,user_ip,len);///YS 要减1，不然对不上号
    }
    else
    {
//        dbug("===============  cant find search host ++++++++++   \n");
    }

    return 0;
}
