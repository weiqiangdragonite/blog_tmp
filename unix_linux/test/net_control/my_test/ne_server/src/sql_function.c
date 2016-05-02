///YS 相关数据库操作的函数
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "config.h"
#include <glib.h>
#include "packet_info.h"

#define SELECT_QUERY_bpf "select Expression from bpf where Enable=1"

char sql_result_buf1[1024];
char sql_result_buf2[1024];
char OK[123];
int k=1;
char mac_tmp[26];
extern void dbug(char *fmt,...);
struct pcre_info s_pcre[pcre_num];
extern void tcp_app_tactics_hase_table_insert(char *  key,char * value);
//extern u_int8_t arp_mac[6];
//extern unsigned int arp_mac[6];
unsigned char  arp_mac2[6];
extern char dst_db_ip[32];
extern char dst_db_name[64];
extern char dst_db_user_name[64];
extern char dst_db_password[64];
/*******************************
从数据库读取各个变量
参数1“char *SELECT_QUERY”，要执行的SQL语句
参数2“SOCK”，是已经连接的数据库句柄
参数3“data”，需要配置的变量
参数4“data_type” 0 是int,1是 字符串
参数5“data_len”变量长度
********************************/
int sql_get_configure_data(char *SELECT_QUERY,MYSQL *sock,void * data,int data_type,int data_len)
{
    MYSQL_ROW row;
    MYSQL_RES *res;
    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }
    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }
    while ((row = mysql_fetch_row(res)))
    {
        if(data_type==1)///YS 1的话就是字符串
        {
            snprintf(data,data_len,"%s",row[0]);
            printf("config data %s  \n",(char*)data);
        }
        if(data_type==0)///YS 0的话就是整型
        {
            *(int*)data=atoi(row[0]);
            printf("config data %d  \n",*(int *)data);
            return  *(int*)data;
        }
        if(data_type==3)///YS 3是特意给MAC用的，从配置表里读取MAC用的 虚构的arp mac地址
        {
            memset(mac_tmp,0,26);
            sprintf(mac_tmp,"%s",row[0]);
            printf("aaaaaaa >>>>   %s\n",row[0]);
           // sscanf(row[0],"%x:%x:%x:%x:%x:%x",&arp_mac2[0],&arp_mac2[1],&arp_mac2[2],&arp_mac2[3],&arp_mac2[4],&arp_mac2[5]);
        }
    }
return 0;
}




/***********************
*SQL初始化，其中里面要连接的IP，是暂时写死在代码里的，可以用个TXT，XML文件写
**********************/
int sql_start(MYSQL *mysql)
{
    mysql_init(mysql);
    mysql_thread_init();

  ///  if (!(mysql_real_connect(mysql,"localhost","root","fanny30399","mydb",0,NULL,CLIENT_MULTI_STATEMENTS)))
  if (!(mysql_real_connect(mysql,dst_db_ip,dst_db_user_name,dst_db_password,dst_db_name,0,NULL,CLIENT_MULTI_STATEMENTS)))

    //if (!(mysql_real_connect(mysql,"192.168.1.222","root","fanny30399","mydb",0,NULL,CLIENT_MULTI_STATEMENTS)))
    {
        fprintf(stderr,"Couldn't connect to rebuild packet server mysql: %s\n",mysql_error(mysql));
        perror("");
        exit(1);
    }
    else
    {
        mysql_set_character_set(mysql,"utf8");///YS 设置以UTF8的格式去连接
        return 1;
    }
}





/**************************
从数据库里读取特征字，然后编译到我们的正则式数组里
参数1“char *SELECT_QUERY”，是要执行的SQL
参数2“MYSQL *sock”，是我们已经连接的数据库的句柄
参数3“GHashTable * hash_table”，是我们要保存特征字的哈希表
**************************/
int sql_get_feature_to_pcre(char *SELECT_QUERY,MYSQL *sock,GHashTable * hash_table)
{
    MYSQL_ROW row;
    MYSQL_RES *res;
    const char *error;
    int erroffset;
    char buff[1024];
    memset(buff,0,1024);
    int i=0;
    dbug("sql get serverip to hash  list");

    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }
    while ((row = mysql_fetch_row(res)))///YS 读数据库查找到的结果
    {
//        printf("ttttttttt  %s\n",row[1]);
        snprintf(s_pcre[i].name,32,row[0]);
        snprintf(s_pcre[i].feature,128,row[1]);
        s_pcre[i].p_pcre = pcre_compile(row[1]  , 0, &error, &erroffset, NULL);///YS PCREAPI，该函数把从数据库读出来的正则式进行编译
        i++;

    }

return 0;
}




/**************************
从数据库里读取协议的端口到哈希表里
参数1“char *SELECT_QUERY”，是要执行的SQL
参数2“MYSQL *sock”，是我们已经连接的数据库的句柄
参数3“GHashTable * hash_table”，是我们要保存特征字的哈希表
**************************/
int sql_get_tcp_port_to_hash(char *SELECT_QUERY,MYSQL *sock,GHashTable * hash_table)
{
    MYSQL_ROW row;
    MYSQL_RES *res;


    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }
    dbug("imp  >>>>>>>>>>>>......sql get serverip to hash  list");

    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }

    while ((row = mysql_fetch_row(res)))
    {
        dbug("imp ooooooooooooooooooo %s\n",row[0]);
        g_hash_table_insert(hash_table, g_strdup(row[0]),(gpointer) 1);///YS 把从数据库查到的结果保存到哈希表里
    }
    mysql_free_result(res);

return 0;
}



/**************************
从数据库里读取服务器IP到哈希表里
参数1“char *SELECT_QUERY”，是要执行的SQL
参数2“MYSQL *sock”，是我们已经连接的数据库的句柄
参数3“GHashTable * hash_table”，是我们要保存特征字的哈希表
参数4“int type”，是我们定义的策略类型
**************************/
int sql_get_server_ip_to_hash(char *SELECT_QUERY,MYSQL *sock,GHashTable * hash_table,int type)
{
    dbug("imp =========================================%d",type);
    MYSQL_ROW row;
    MYSQL_RES *res;///YS 不同的RES保存不同的结果，重用一个RES的话，会造成丢失
    MYSQL_RES *res2;
    MYSQL_RES *res3;
    char buff[1024];
    memset(buff,0,1024);
    char key[32];
    char app_table_name[32];
    char user_tmp[64];
      char app_sel[512];
    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }
    while ((row = mysql_fetch_row(res)))
    {
//        printf("rrrrrrr   %s\n",row[0]);

        if(strstr(row[0],"pp_"))///YS 如果这个是一个APP_IM,或者是APP_GAME等等，而不是URL，因为URL是没有服务器这一说法的，只有聊天，游戏那些才会有的
        {
            snprintf(app_table_name,sizeof(app_table_name),"%s",row[0]);

            if(!strcmp(row[1],"all"))///YS 如果是 “all” 的话
            {
               // dbug("imp this is all=====================================start======================");///r1  all   r2  ip  r0 app_im

                snprintf(user_tmp,sizeof(user_tmp),"%s",row[2]);///YS 不同的参数，得出的结果不一样，具体看数据库跑

                snprintf(app_sel,sizeof(app_sel),"select Name from app_table where Tactics_type='%s'",row[0]);///YS 因为是ALL，所以要查找所有名字
                if(mysql_query(sock,app_sel))
                {
                    fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
                }
                if (!(res3=mysql_store_result(sock)))
                {
                    fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
                    exit(1);
                }

                while ((row = mysql_fetch_row(res3)))
                {
                    if(!strcmp(row[0],"all")) continue;
                    if(type ==1 )///YS white list
                    {
                        snprintf(key,sizeof(key),"%s%s",row[0],user_tmp);///YS ip+ip(服务器IP，用户IP）
                        tcp_app_tactics_hase_table_insert(key,"white_list");///YS 这个函数里的哈希表是用作判断是哪一种策略的，下同
                    }
                    if(type ==2 )///YS user black list
                    {
                        snprintf(key,sizeof(key),"%s%s",row[0],user_tmp);///YS ip+ip(服务器IP，用户IP）
                        tcp_app_tactics_hase_table_insert(key,"person_list");
                    }
                    if(type ==3 )///YS group black list
                    {
                        snprintf(key,sizeof(key),"%s%s",row[0],user_tmp);///YS ip+ip(服务器IP，用户IP）
                        tcp_app_tactics_hase_table_insert(key,"group_list");
                    }
                    if(type ==4 )///YS whole list
                    {
                        tcp_app_tactics_hase_table_insert((row[0]),"whole_list");///YS 只插入服务器IP就可以了
                    }
                    memset(app_sel,0,sizeof(app_sel));
                    snprintf(app_sel,sizeof(app_sel),"select IP,Port from %s where name='%s'",app_table_name,row[0]);///ys 找相应的服务器IP，或者端口

                    if(mysql_query(sock,app_sel))
                    {
                        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
                        exit(1);
                    }

                    if (!(res2=mysql_store_result(sock)))
                    {
                        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
                        exit(1);
                    }

                    while ((row = mysql_fetch_row(res2)))
                    {
                        strcat(buff,user_tmp);
                        if(row[1])
                        {
                            strcat(buff,row[1]);///YS 如果有端口的话，就不用去看IP了，只要构成KEY：192.168.1.11123，VALUE：telnet就行
                        }
                        else
                        {
                            strcat(buff,row[0]);///YS 否则：组成IP+IP的型式，但VALUE是协议名字 192.168.1.111123.23.42.12---->yy
                        }
                        //dbug("222222222   %s",buff);
                        dbug("111111111   key ====> %s",buff);
                        g_hash_table_insert(hash_table, g_strdup(buff), (gpointer)1);
                        memset(buff,0,1024);
                    }
                }
               // dbug("imp this is all=====================================end======================");
                continue;
            }

            ///YS  上面的针对APP——ALL的设置的，下面是针对APP_IM,YY而不是APP_IM,ALL这种型式




            memset(key,0,strlen(key));
            ///YS 下面要注意一点，当白名单与黑名单里有相同的规则，我们只处理白名单就够了
            if(type ==1  )///ys white list
            {
                strcat(key,row[1]);
                strcat(key,row[2]);
                snprintf(user_tmp,sizeof(user_tmp),"%s",row[2]);
                tcp_app_tactics_hase_table_insert(key,"white_list");
            }
            if(type ==2 )///ys person black list
            {
                strcat(key,row[1]);///detail
                strcat(key,row[2]);///ip
                snprintf(user_tmp,sizeof(user_tmp),"%s",row[2]);///ip
                tcp_app_tactics_hase_table_insert(key,"person_list");
            }
            if(type ==3 )///ys gid black list
            {

                strcat(key,row[1]);
                strcat(key,row[2]);
                snprintf(user_tmp,sizeof(user_tmp),"%s",row[2]);
                //dbug("key is : %s",key);
                tcp_app_tactics_hase_table_insert(key,"group_list");
            }
            if(type ==4 )///ys ALL black list
            {
                //dbug("4");
                strcat(key,row[1]);
                memset(user_tmp,0,64);
                //dbug("key is : %s",key);
                tcp_app_tactics_hase_table_insert((key),"whole_list");
            }
						///YS 因为我们是以ip+ip 的型式来做  hash KEY，所以，上面的代码只是拿相应的名字，如拿到YY，然后下面的代码就去拿YY的IP，然后YY的IP＋IP，组成KEY，再写入哈希表

            char app_sel[512];
            snprintf(app_sel,sizeof(app_sel),"select IP,Port from %s where name='%s'",row[0],row[1]);//ys 找相应的服务器IP，或者端口 ys//

            // dbug(app_sel);
            if(mysql_query(sock,app_sel))
            {
                fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
                exit(1);
            }

            if (!(res2=mysql_store_result(sock)))
            {
                fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
                exit(1);
            }

            while ((row = mysql_fetch_row(res2)))
            {
                strcat(buff,user_tmp);
                if(row[1])
                {
                    strcat(buff,row[1]);///YS  192.168.1.11123-->telnet
                }
                else
                {
                    strcat(buff,row[0]);///YS 192.168.1.111123.23.42.12---->yy
                }
                dbug("111111111   key ====> %s",buff);
                g_hash_table_insert(hash_table, g_strdup(buff), (gpointer)1);
                memset(buff,0,1024);
            }

        }

    }
        dbug("imp =========================================%d",type);
return 0;
}

/*
int sql_get_server_ip_to_hash2(char *SELECT_QUERY,MYSQL *sock,int http_type,GHashTable * hash_table)
{
    MYSQL_ROW row;
    MYSQL_RES *res;
    char buff[1024];
    memset(buff,0,1024);

    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }

    while ((row = mysql_fetch_row(res)))
    {

        if(!strcmp(row[0],"URL_news"))
        {
            mysql_query(sock,GET_URL_news_list);
            res2=mysql_store_result(sock);
            while(row=mysql_fetch_row(res2))
            {

                g_hash_table_insert(hash_table,g_strdup(row[0]),"ALL");
            }
            //mysql_free_result(res);
            continue;
        }
        if(!strcmp(row[0],"URL_adult"))
        {
            mysql_query(sock,GET_URL_adult_list);
            res2=mysql_store_result(sock);
            while(row=mysql_fetch_row(res2))
            {

                g_hash_table_insert(hash_table,g_strdup(row[0]),"ALL");
            }
            //mysql_free_result(res);
            continue;
        }
        if(!strcmp(row[0],"URL_music_movie"))
        {
            mysql_query(sock,GET_URL_music_movie_list);
            res2=mysql_store_result(sock);
            while(row=mysql_fetch_row(res2))
            {

                g_hash_table_insert(hash_table,g_strdup(row[0]),"ALL");
            }
            //mysql_free_result(res);
            continue;
        }
        //g_char * str;
        //str=g_strconcat(row[0],row[1]);
        strcat(buff,row[0]);
        strcat(buff,row[1]);
        //dbug("row00000000   %s",buff);

        g_hash_table_insert(hash_table, g_strdup(buff), g_strdup(row[1]));
        dbug(buff);
        memset(buff,0,1024);
    }
    mysql_free_result(res);
    //mysql_free_result(res2);
    return 0;   //
}
*/


/*****************************
*从数据库里获取网址到哈希表中
*参数3"http_type"，是哈希表类型,也就是哪个HTTP哈希表，HTTP也有分白名单，黑名单之类的
SELECT_QUERY，是查询数据库的SQL（看你要查的是哪个类别的URL）
******************************/
int sql_get_website_to_hash(char *SELECT_QUERY,MYSQL *sock,int http_type,GHashTable * hash_table)
{

    MYSQL_ROW row;
    MYSQL_ROW row2;
    MYSQL_RES *res;
    MYSQL_RES *res2;
    char buff[1024];
    char sqlstr[1024];
    char user_ip_or_gid[24];

    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }


    while( (row = mysql_fetch_row(res)))
    {

        if(http_type==6||http_type==5)
        {
            dbug("  33key===> %s   value===>url",row[0]);
            g_hash_table_insert(hash_table, g_strdup(row[0]), "url");
            continue;
        }


        memset(user_ip_or_gid,0,sizeof(user_ip_or_gid));
        if(strstr(row[0],"rl_"))
        {

            if(strlen(row[1]))
            {
                if(http_type!=4)
                {
                    snprintf(user_ip_or_gid,sizeof(user_ip_or_gid),"%s",row[1]);
                }
            }
            snprintf(sqlstr,sizeof(sqlstr),SELECT_FROM_URL_GROUP,row[0]);
            mysql_query(sock,sqlstr);
            res2=mysql_store_result(sock);
            while((row2=mysql_fetch_row(res2)))
            {
                memset(buff,0,sizeof(buff));
                if(strlen(user_ip_or_gid))
                {
                    strcat(buff,row2[0]);
                    strcat(buff,user_ip_or_gid);
                }
                else
                {
                    strcat(buff,row2[0]);
                }
                dbug("  22key===> %s   value===>url",buff);
                g_hash_table_insert(hash_table,g_strdup(buff),"url");
            }
            mysql_free_result(res2);
            continue;
        }
        memset(buff,0,sizeof(buff));
        strcat(buff,row[0]);
        if(row[1]&&http_type!=4)
        {
            strcat(buff,row[1]);
        }
        dbug("  key===> %s   value===>url",buff);
        g_hash_table_insert(hash_table, g_strdup(buff), "url");
    }
   // if(http_type==4)exit(0);
    mysql_free_result(res);
    return 0;
}


/*************************
*从数据库里获取用户到哈希表中
*参数：SELECT_QUERY，是查询数据库的SQL（看你要查的是哪个类别的URL）
****************************/
int sql_get_user_to_hash(char *SELECT_QUERY,MYSQL *sock,GHashTable * hash_table)
{
    MYSQL_ROW row;
    MYSQL_RES *res;
    if(mysql_query(sock,SELECT_QUERY))
    {
        dbug(SELECT_QUERY);
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }

    while ((row = mysql_fetch_row(res)))
    {
        g_hash_table_insert(hash_table, g_strdup(row[0]), g_strdup(row[1]));
    }
    mysql_free_result(res);
    return 0;   //
}

/**********************
*mysql退出函数
************************/
void mysql_end(MYSQL_RES *res,MYSQL *sock)
{
    mysql_free_result(res);
    mysql_close(sock);
    mysql_thread_end();
}
