///YS 很多审计完成后，把结果写入数据库的函数都是放在这里的

#include <stdio.h>
#include "time.h"
#include "packet_info.h"
#include "config.h"
#include <mysql/mysql.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

extern MYSQL mysql;
extern int uri_iconv (char *args);
extern int email_content_iconv (char *args);


/************************
流量记录写入到数据库
参数1，是用户的IP
参数2，是用户的流量
参数3，是“是否阻断”
************************/
int  put_flow_record_into_db(char *user_ip,long flow,int block)
{

    MYSQL_RES *res;
    MYSQL_ROW row;
    char sqlbuff[1024*100];
    memset(sqlbuff,0,sizeof(sqlbuff));
    snprintf(sqlbuff,sizeof(sqlbuff),GET_USER_NAME_UID,user_ip); ///YS 跟据IP，或取个人信息

    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        return 0;
    }
    res=mysql_store_result(&mysql);

    if(mysql_num_rows(res)<1)
    {
        mysql_free_result(res);
        return 0;
    }
    row=mysql_fetch_row(res);


    snprintf(sqlbuff,sizeof(sqlbuff),INSERT_FLOW,row[0],user_ip,row[1],row[2],flow,block,row[3]);  ///YS 把流量信息插入到数据库;

    if(mysql_query(&mysql,sqlbuff))
    {
        //fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        memset(sqlbuff,0,sizeof(sqlbuff));
        snprintf(sqlbuff,sizeof(sqlbuff),UPDATE_FLOW,flow,block,user_ip);///YS 如果数据已经存在了，就更新流量信息，而不是插入;

        if(mysql_query(&mysql,sqlbuff))
        {

            fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        }
        else
        {
            mysql_free_result(res);
              return 0;
        }

    }

    mysql_free_result(res);
    return 0;
}



/************************
POP3 content  into db
************************/
int put_email_POP3_into_db(char *user_ip,char *content,char *title,char *sender,char *receiver)
{

    MYSQL_RES *res;
    MYSQL_ROW row;
    char sqlbuff[1024*100];

    snprintf(sqlbuff,sizeof(sqlbuff),GET_USER_NAME_UID,user_ip);
    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        return 0;
    }
    res=mysql_store_result(&mysql);
     if(mysql_num_rows(res)<1)
     {
         mysql_free_result(res);
         return 0;
     }
    row=mysql_fetch_row(res);

    time_t  now;
    struct tm *tm_now;
    char datetime[64];
    time(&now);
    tm_now=localtime(&now);
    strftime(datetime,64,"%Y-%m-%d %H:%M:%S",tm_now);
    snprintf(sqlbuff,sizeof(sqlbuff),INSERT_AUDIT_EMAIL_POP3,row[0],row[1],user_ip,datetime,row[2],content,receiver,sender,title,1);///YS 这里要注意，之所以要+9，是因为我们的网站目录是在/home/www下，跳过这个绝对路经成相对路经
    dbug("kkkkkkkkkkkkkkk  insert mysql str  ====>  %s  \n",sqlbuff);
    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        //exit(1);
    }

mysql_free_result(res);
    return 0;
}





/************************
论坛审计结果写入到数据库
参数1，是用户的IP
参数2，是发贴回贴的内容
参数3，是论坛的域名
参数4，是用户在论坛里的用户名
参数5，是贴子的title
参数6，是贴子的URL
************************/
int     put_bbs_record_into_db(char *user_ip,char *content,char *host,char *username,char * title,char *url)
{

    MYSQL_RES *res;
    MYSQL_ROW row;
    char key_only[244];
    char sqlbuff[1024*100];


    snprintf(sqlbuff,sizeof(sqlbuff),GET_USER_NAME_UID,user_ip);///YS 跟据IP，或取个人信息
    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        return 0;
    }
    res=mysql_store_result(&mysql);
     if(mysql_num_rows(res)<1)
     {
         mysql_free_result(res);
         return 0;
     }
    row=mysql_fetch_row(res);

    time_t  now;
    struct tm *tm_now;
    char datetime[64];
    char datetime2[64];
    char title_short[64];
    time(&now);
    tm_now=localtime(&now);
    strftime(datetime,64,"%Y-%m-%d %H:%M:%S",tm_now);///YS 获取当前时间
    strftime(datetime2,64,"%Y-%m-%d %H",tm_now);///YS 获取当前时间，该时间用于KEYONLY
    if(!strcmp(title,"null"))
    {
        sprintf(title,"null(%s)",datetime);
    }
    snprintf(title_short,18,"%s",title);
    snprintf(key_only,sizeof(key_only),"%s %s %s %s",datetime2,title_short,user_ip,host);///YS KEYONLY，是为了防止同一样记录的多次出现
    snprintf(sqlbuff,sizeof(sqlbuff),INSERT_AUDIT_BBS,row[0],user_ip,row[1],datetime,content,key_only,url,username,title,host,row[2]);

    if(mysql_query(&mysql,sqlbuff))///YS 执行插入语句
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        //exit(1);
    }
    mysql_free_result(res);
    return 0;
}

/************************
聊天审计结果写入到数据库
参数1，是接收的用户名
参数2，是聊天内容
参数3，是用户IP
参数4，是聊天的类型，是QQ，飞信，还是MSN
参数5，是发送人的名字
参数6，是信息类型，用户登陆，还是发送信息，还是接收信息
************************/
int insert_into_db_im(char *receive,char *content,char *user_ip,char * Im_type,char *Sender,char *Comment)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char sqlbuff[1024*100];

    char new_str[1024*300];
    mysql_real_escape_string(&mysql,new_str,content,strlen(content));
    snprintf(sqlbuff,sizeof(sqlbuff),GET_USER_NAME_UID,user_ip);

    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        return 0;
    }
    res=mysql_store_result(&mysql);
     if(mysql_num_rows(res)<1)
     {
         mysql_free_result(res);
         return 0;
     }
    row=mysql_fetch_row(res);

    time_t  now;
    struct tm *tm_now;
    char datetime[64];
    time(&now);
    tm_now=localtime(&now);
    strftime(datetime,64,"%Y-%m-%d %H:%M:%S",tm_now);
    snprintf(sqlbuff,sizeof(sqlbuff),INSERT_AUDIT_IM,row[0],user_ip,row[1],Im_type,datetime,Sender,receive,new_str,Comment,row[2]);
dbug("%s\n",sqlbuff);
    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        //exit(1);
    }
    mysql_free_result(res);
    return 0;
}


/************************
对URL编码进行解码转换成中文，本来是用可以用的，不过后天发现，用PHP来做更好，我只负责把数据写入到数据库，PHP那边读出来时转换一下就好了
参数1，要转换的内容
************************/
int code_convert_email(char *keyword)
{
    return 0;
    int len_start,i;
    char hex_file_name[1024*100];
    memset(hex_file_name,0,1024*100);
    len_start=0;
    for(  i=0; i<strlen(keyword); i++  )//01name.jpg
    {

        if(isprint(keyword[i]))
        {
            hex_file_name[len_start]=keyword[i];
            len_start++;
        }
        else
        {
            char a[3];
            sprintf(a,"%%%X",(unsigned char)keyword[i]);
            strcat(hex_file_name,a);
            len_start=len_start+3;
        }
    }

    // dbug(" %d  #################    %s\n",strlen(keyword),hex_file_name);

    memset(keyword,0,strlen(keyword));
    strcpy(keyword,hex_file_name);

    email_content_iconv(keyword);
    return 0 ;
}


/************************
邮件审计结果写入到数据库
参数1，是用户IP
参数2，是邮件内容
参数3，是邮件网址
参数4，是发件人
参数5，是收件人
参数6，是邮件标题
************************/
int put_email_record_into_db(char *user_ip,char *content,char *host,char *s_email,char * r_email,char *title)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char key_only[244];
    char sqlbuff[1024*100];
    char receiver_email_address_5[5];///YS 这前五个，是用KEYONLY那里的，防止因为r_email的结尾有不能识别的符号，而导致出现多条记录
    snprintf(receiver_email_address_5,5,"%s",r_email);

    char new_str[1024*300];
    mysql_real_escape_string(&mysql,new_str,content,strlen(content));///YS MYSQLAPI，这一句很重要，可以把CONTENT中的回车，分号等转义，保存到数据库中

    snprintf(sqlbuff,sizeof(sqlbuff),GET_USER_NAME_UID,user_ip);///YS 查找用户信息

    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        return 0;
    }
    res=mysql_store_result(&mysql);
     if(mysql_num_rows(res)<1)
     {mysql_free_result(res);
     return 0;
     }
    row=mysql_fetch_row(res);

    time_t  now;
    struct tm *tm_now;
    char datetime[64];
    char datetime2[64];
    time(&now);
    tm_now=localtime(&now);
    strftime(datetime,64,"%Y-%m-%d %H:%M:%S",tm_now);///YS 获取时间
    strftime(datetime2,64,"%Y-%m-%d %H",tm_now);
    snprintf(key_only,sizeof(key_only),"%s %s %s %s %s",datetime2,title,user_ip,s_email,receiver_email_address_5);///YS KEYONLY
    snprintf(sqlbuff,sizeof(sqlbuff),INSERT_AUDIT_EMAIL,key_only,row[0],row[1],user_ip,host,datetime,new_str,r_email,s_email,title,row[2]);///YS 写入数据库
    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        //exit(1);
    }
    mysql_free_result(res);
    return 0;
}

/************************
上传下载审计结果写入到数据库
参数1，是用户IP
参数2，是文件名
参数3，是上传或下载
************************/
int put_http_updownload_into_db(char * user_ip,char *filename,int u_or_d)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char sqlbuff[1024*200];
    snprintf(sqlbuff,sizeof(sqlbuff),GET_USER_NAME_UID,user_ip);
    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        return 0;
    }
    res=mysql_store_result(&mysql);
     if(mysql_num_rows(res)<1)
     {
         mysql_free_result(res);
         return 0;

     }
    row=mysql_fetch_row(res);

    time_t  now;
    struct tm *tm_now;
    char datetime[64];
    time(&now);
    tm_now=localtime(&now);
    strftime(datetime,64,"%Y-%m-%d %H:%M:%S",tm_now);
    //dbug(" %s >>>>>>   %s \n",row[1],row[0]);
    memset(sqlbuff,0,sizeof(sqlbuff));
    if(u_or_d==4)snprintf(sqlbuff,sizeof(sqlbuff),INSERT_AUDIT_U_OR_D,row[0],user_ip,row[1],datetime,2,filename+9,row[2]);///YS 这里要注意，之所以要+9，是因为我们的网站目录是在/home/www下，跳过这个绝对路经成相对路经
    else snprintf(sqlbuff,sizeof(sqlbuff),INSERT_AUDIT_U_OR_D,row[0],user_ip,row[1],datetime,u_or_d,filename+9,row[2]);
    // dbug("insert &mysql str  ====>  %s  \n",sqlbuff);
    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        //exit(1);
    }
    mysql_free_result(res);

    return 0;
}


/************************
网址浏览审计结果写入到数据库
参数1，是用户IP
参数2，是网址的HOST
参数3，是网页的REFERER
************************/
int put_http_host_into_db(char * user_ip,char *host,char *referer)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char hex_file_name[1440];
    int len_start;
    char sqlbuff[1024*100];
    memset(hex_file_name,0,1440);
    len_start=0;
    snprintf(sqlbuff,sizeof(sqlbuff),GET_USER_NAME_UID,user_ip);
    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        return 0;
    }
    res=mysql_store_result(&mysql);
     if(mysql_num_rows(res)<1)///YS 如果这个IP不存在的话，就返回
     {
         mysql_free_result(res);
         return 0;
     }
    row=mysql_fetch_row(res);
    time_t  now;
    struct tm *tm_now;
    char datetime[64];
    char datetime2[64];
    time(&now);
    tm_now=localtime(&now);
    strftime(datetime,64,"%Y-%m-%d %H:%M:%S",tm_now);
    strftime(datetime2,64,"%Y-%m-%d %H:%M",tm_now);
    char key_only[244];
    snprintf(key_only,sizeof(key_only),"%s %s %s",user_ip,host,datetime2);
    snprintf(sqlbuff,sizeof(sqlbuff),INSERT_AUDIT_HTTP_HOST,row[0],user_ip,row[1],datetime,host,key_only,referer,row[2]);///YS 把host,referer都插入到数据库里
    //dbug("insert &mysql str  ====>  %s  \n",sqlbuff);
    if(mysql_query(&mysql,sqlbuff))
    {
        //  fprintf(stderr,"Query failed (%s)\n",mysql_error(&&mysql));
        //exit(1);
    }
mysql_free_result(res);
    return 0;
}

/*int put_http_host_into_db(char * user_ip,char *host,char *referer)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char hex_file_name[1440];
    int len_start,len_end,i;
    char sqlbuff[1024*100];
    ///////////////////filename
    ///change the hex to char
    memset(hex_file_name,0,1440);
    len_start=0;


    snprintf(sqlbuff,sizeof(sqlbuff),GET_USER_NAME_UID,user_ip);
    ///get username and uid
    ///insert to sql
    if(mysql_query(&&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&&mysql));
    }
    res=mysql_store_result(&&mysql);
    row=mysql_fetch_row(res);
    ///insert the key word into db;
    time_t  now;
    struct tm *tm_now;
    char datetime[64];
    char datetime2[64];
    time(&now);
    tm_now=localtime(&now);
    strftime(datetime,64,"%Y-%m-%d %H:%M:%S",tm_now);
    strftime(datetime2,64,"%Y-%m-%d %H:%M",tm_now);
    char key_only[244];
    snprintf(key_only,sizeof(key_only),"%s %s %s",user_ip,host,datetime2);
    snprintf(sqlbuff,sizeof(sqlbuff),INSERT_AUDIT_HTTP_HOST,row[0],user_ip,row[1],datetime,host,key_only,referer);
    dbug("insert &mysql str  ====>  %s  \n",sqlbuff);
    if(mysql_query(&&mysql,sqlbuff))
    {
        //  fprintf(stderr,"Query failed (%s)\n",mysql_error(&&mysql));
        //exit(1);
    }
}*/

/************************
邮件浏览审计结果写入到数据库
参数1，是用户IP
参数2，是用户浏览邮件所生成的网页
************************/
int put_email_read_into_db(char *user_ip,char *filename)
{

    MYSQL_RES *res;
    MYSQL_ROW row;
    char sqlbuff[1024*100];

    snprintf(sqlbuff,sizeof(sqlbuff),GET_USER_NAME_UID,user_ip);
    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        return 0;
    }
    res=mysql_store_result(&mysql);
     if(mysql_num_rows(res)<1)
     {
         mysql_free_result(res);
         return 0;
     }
    row=mysql_fetch_row(res);

    time_t  now;
    struct tm *tm_now;
    char datetime[64];
    time(&now);
    tm_now=localtime(&now);
    strftime(datetime,64,"%Y-%m-%d %H:%M:%S",tm_now);
    snprintf(sqlbuff,sizeof(sqlbuff),INSERT_AUDIT_EMAIL_READ,row[0],row[1],user_ip,datetime,filename+9,row[2]);///YS 这里要注意，之所以要+9，是因为我们的网站目录是在/home/www下，跳过这个绝对路经成相对路经
    dbug("kkkkkkkkkkkkkkk  insert mysql str  ====>  %s  \n",sqlbuff);
    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        //exit(1);
    }

mysql_free_result(res);
    return 0;
}



/************************
搜索审计结果写入到数据库
参数1，是用户IP
参数2，是关键字
参数3，是搜索引擎
************************/
int put_search_record_into_db(char *user_ip,char *keyword,char *search_eng)
{

    MYSQL_RES *res;
    MYSQL_ROW row;
    char hex_file_name[1440];
    int len_start,i;
    char sqlbuff[1024*100];
    memset(hex_file_name,0,1440);
    len_start=0;

    for(  i=0; i<strlen(keyword); i++  )///YS 这里进行一个简单的URL编码转换
    {
        if(keyword[i]=='+')
        {
            keyword[i]=' ';
        }
        if(isprint(keyword[i]))
        {
            hex_file_name[len_start]=keyword[i];
            len_start++;
        }
        else
        {
            char a[3];
            sprintf(a,"%%%X",(unsigned char)keyword[i]);
            strcat(hex_file_name,a);
            len_start=len_start+3;
        }
    }

    memset(keyword,0,sizeof(keyword));
    strcpy(keyword,hex_file_name);

    //uri_iconv(keyword);
    //#define GET_USER_NAME_UID
    //#define INSERT_AUDIT_SEARCH
    snprintf(sqlbuff,sizeof(sqlbuff),GET_USER_NAME_UID,user_ip);
    ///get username and uid
    ///insert to sql
    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        return 0;
    }
    res=mysql_store_result(&mysql);
     if(mysql_num_rows(res)<1)
     {mysql_free_result(res);
     return 0;
     }
    row=mysql_fetch_row(res);

    ///insert the key word into db;
    time_t  now;
    struct tm *tm_now;
    char datetime[64];
    char datetime2[64];
    time(&now);
    tm_now=localtime(&now);
    strftime(datetime,64,"%Y-%m-%d %H:%M:%S",tm_now);
    strftime(datetime2,64,"%Y-%m-%d %H",tm_now);
    char key_only[244];
    snprintf(key_only,sizeof(key_only),"%s %s %s  %s",user_ip,keyword,search_eng,datetime2);
    snprintf(sqlbuff,sizeof(sqlbuff),INSERT_AUDIT_SEARCH,key_only,row[0],row[1],user_ip,search_eng,datetime,keyword,row[2]);
    if(mysql_query(&mysql,sqlbuff))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        //exit(1);
    }
    mysql_free_result(res);

    return 0;
}

