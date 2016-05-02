///YS 数据库的相关操作
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "config.h"
#include <glib.h>
#include "packet_info.h"
#include <time.h>

#define SELECT_QUERY_bpf "select Expression from bpf where Enable=1"///YS 把BPF的过滤条件从数据库里读出来

//MYSQL_FIELD *fd ;     //包含字段信息的结构
char sql_result_buf1[1024];
char sql_result_buf2[1024];
time_t timep;
extern char match_file_type[support_file_num][32];
struct pcre_info s_pcre[pcre_num];
struct pcre_info_im app_im_pcre[pcre_num];

extern GHashTable *file_name_ack_list;
extern struct user_flow userf[255];
extern pthread_mutex_t *mutex_for_sql_write;
extern  char filename_type[1024*1024];

struct file_type file_header[Audit_count];
extern int file_type_sum;
char OK[123];
int k=1;
char mac_tmp[26];

extern char dst_db_ip[32];
extern char dst_db_name[64];
extern char dst_db_user_name[64];
extern char dst_db_password[64];
 unsigned char  arp_mac2[6];
/*
*SQL初始化
*/
int sql_start(MYSQL *mysql)
{
    mysql_library_init(0,NULL,NULL);
    mysql_init(mysql);
    if (!(mysql_real_connect(mysql,"localhost",dst_db_user_name,dst_db_password,dst_db_name,0,NULL,CLIENT_MULTI_STATEMENTS)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s",mysql_error(mysql));
        perror("");
        mysql_close(mysql);
        exit(1);
    }
    else
    {
        mysql_set_character_set(mysql,"utf8");///YS 设置UTF8
        return 1;
    }
}


/*void get_file_name_from_db(MYSQL *sock,unsigned long int  ack,char * filename,char *stream)
{
    char sql_str[file_name_str_len];
    MYSQL_ROW row;
    MYSQL_RES *res;

    sprintf(sql_str,GET_FILE_NAME_USE_ACK_OR_STREAM,ack,stream);

    pthread_mutex_lock(mutex_for_sql_write);
    if(mysql_query(sock,sql_str))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }
    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }
    row = mysql_fetch_row(res);
    pthread_mutex_unlock(mutex_for_sql_write);

    if(row)
    {
        strcpy(filename,row[0]);
        memset(sql_str,0,file_name_str_len);
        sprintf(sql_str,"%x",ack);
        //YS putinto the hash  YS//
        g_hash_table_insert(file_name_ack_list, g_strdup(sql_str),g_strdup(row[0]));
        sprintf(sql_str,DEL_FILE_NAME,filename);
    pthread_mutex_lock(mutex_for_sql_write);
        if(mysql_query(sock,sql_str))
        {
            fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
            mysql_free_result(res);
            return 0;//exit(1);
        }
         pthread_mutex_unlock(mutex_for_sql_write);

        mysql_free_result(res);
        return 1;
    }
    mysql_free_result(res);
    return 0;   //

}


void sql_put_name_into_DB(MYSQL *sock,char * file_name,unsigned long int  ack,char *stream)
{
    int i=0;
    time(&timep);
    MYSQL_ROW row;
    MYSQL_RES *res;
    char sql_str[1024];
    sprintf(sql_str,PUT_FILE_NAME_2_DB,file_name,ack,timep,stream);
    //  dbug(">>>>>>   %s\n",sql_str);

    pthread_mutex_lock(mutex_for_sql_write);
    if(mysql_query(sock,sql_str))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        return 0;// exit(1);
    }
    pthread_mutex_unlock(mutex_for_sql_write);

}
*/
/*
#define white_list 1
#define person_list 2
#define group_list 3
#define whole_list 4
    sql_get_server_ip_to_hash(&mysql,tcp_hash_person_list,person_list);
    sql_get_server_ip_to_hash(&mysql,tcp_hash_group_list,group_list);
    sql_get_server_ip_to_hash(&mysql,tcp_hash_whole_list,whole_list);
*/



/***********************
从数据库读出来配置数据到程序中
参数1，SQL语句
参数2，SQL已连接的句柄
参数3，要保存到的对像的指针
参数4，对像的数据类型
参数5，对像的长度
***********************/
int sql_get_configure_data(char *SELECT_QUERY,MYSQL *sock,void* data,int data_type,int data_len)///data_type 0 int,1 char
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
    //char name_temp[128];
    while((row = mysql_fetch_row(res)))//ys ready for
    {
        if(data_type==3)///YS 3是特意给MAC用的，从配置表里读取MAC用的
        {
            memset(mac_tmp,0,26);
            memcpy(mac_tmp,row[0],strlen(row[0]));
            printf("aaaaaaa >>>>   %s\n",mac_tmp);
            sscanf(mac_tmp,"%x:%x:%x:%x:%x:%x",&arp_mac2[0],&arp_mac2[1],&arp_mac2[2],&arp_mac2[3],&arp_mac2[4],&arp_mac2[5]);
            mysql_free_result(res);
            return 0;
        }
        else
        {
            if(data_type)
            {
                snprintf(data,data_len,"%s",row[0]);
                //dbug("config data %s  \n",data);
            }
            else
            {
                *((int *)data)=atoi(row[0]);
                //dbug("config data %d  \n",data);
                mysql_free_result(res);
                if(data) return *((int *)data);
                else return 0;
            }
        }


    }
    mysql_free_result(res);
    return 0;
}

/*********************
把MAC插入到FLOW表中
**********************/
int sql_put_mac_to_flow_table(char *SELECT_QUERY,MYSQL *sock)
{
    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }
}





/*********************
把聊天应用的特征，从数据库中读出来
参数1，SQL语句
参数2，已连接的SQL句柄
**********************/
int sql_get_app_im_feature(char *SELECT_QUERY,MYSQL *sock)
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
    //char name_temp[128];
    int i=0;
    while ((row = mysql_fetch_row(res)))//ys ready for
    {
        if(row[1]) snprintf(app_im_pcre[i].name,32,"%s",row[1]);
        if (row[2])snprintf(app_im_pcre[i].feature,256,"%s",row[2]);
        if (row[3])app_im_pcre[i].case_value=atoi(row[3]);
        if (row[4])snprintf(app_im_pcre[i].content_feature_s,32,"%s",row[4]);
        if (row[5])snprintf(app_im_pcre[i].content_feature_e,32,"%s",row[5]);
        if (row[6])snprintf(app_im_pcre[i].sender_s,32,"%s",row[6]);
        if (row[7])
        {
            if(!strcmp(row[7],"\\r\\n"))///把\R\N换成真正的\R\N
            {
                snprintf(app_im_pcre[i].sender_e,32,"%s","\x0D\x0A");
            }
            else
            {

                snprintf(app_im_pcre[i].sender_e,32,"%s",row[7]);
            }
        }
        if (row[8])snprintf(app_im_pcre[i].receiver_s,64,"%s",row[8]);
        if (row[9])
        {
            if(!strcmp(row[9],"\\r\\n"))
            {
                snprintf(app_im_pcre[i].receiver_e,64,"%s","\x0D\x0A");
            }
            else
            {
                snprintf(app_im_pcre[i].receiver_e,64,"%s",row[9]);
            }
        }



        if (row[10])snprintf(app_im_pcre[i].login_user_s,64,"%s",row[10]);

        if (row[11])
        {
            if(!strcmp(row[11],"\\r\\n"))
            {
                snprintf(app_im_pcre[i].login_user_e,64,"%s","\x0D\x0A");
            }
            else
            {

                snprintf(app_im_pcre[i].login_user_e,64,"%s",row[11]);
            }
        }

        if (row[12])snprintf(app_im_pcre[i].login_user_ex,64,"%s",row[12]);
        if (row[13])snprintf(app_im_pcre[i].content_feature_ex,64,"%s",row[13]);
        if (row[14])snprintf(app_im_pcre[i].receiver_ex,64,"%s",row[14]);
        if (row[15])snprintf(app_im_pcre[i].sender_ex,64,"%s",row[15]);


        ///for in*************************************************
        if (row[16])snprintf(app_im_pcre[i].in_sender_s,64,"%s",row[16]);
        if (row[17])
        {
            if(!strcmp(row[17],"\\r\\n"))
            {
                snprintf(app_im_pcre[i].in_sender_e,64,"%s","\x0D\x0A");
            }
            else
            {

                snprintf(app_im_pcre[i].in_sender_e,64,"%s",row[17]);
            }
        }
        if (row[18])snprintf(app_im_pcre[i].in_receiver_s,64,"%s",row[18]);
        if (row[19])
        {
            if(!strcmp(row[19],"\\r\\n"))
            {
                snprintf(app_im_pcre[i].in_receiver_e,64,"%s","\x0D\x0A");
            }
            else
            {
                snprintf(app_im_pcre[i].in_receiver_e,64,"%s",row[19]);
            }
        }
        if (row[20])snprintf(app_im_pcre[i].in_ex,64,"%s",row[20]);
        if (row[21])
        {
            if(!strcmp(row[21],"\\r\\n\\r\\n"))
            {
                snprintf(app_im_pcre[i].in_content_feature_s,64,"%s","\x0D\x0A\x0D\x0A");
            }
            else
            {
                snprintf(app_im_pcre[i].in_content_feature_s,64,"%s",row[21]);
            }
        }

        if (row[22])snprintf(app_im_pcre[i].in_content_feature_e,64,"%s",row[22]);

        if (row[23])snprintf(app_im_pcre[i].ex_1,64,"%s",row[23]);


        i++;
    }
    mysql_free_result(res);

    return 0;
}



/*********************
获取要过滤的文件类型（.JPG  .RAR .ZIP），以及他们相应的文件头特征
参数1，SQL语句
参数2，已连接的SQL句柄
**********************/
int sql_get_file_name_feature(char *SELECT_QUERY,MYSQL *sock)
{
    const char *error;
    int erroffset;
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
    //char name_temp[128];
    int i=0;
    while ((row = mysql_fetch_row(res)))//ys ready for
    {
        snprintf(file_header[i].filetype,8,"%s",row[0]);
        snprintf(file_header[i].filetype_header,256,"%s",row[1]);

        dbug("%s   %s\n",file_header[i].filetype,file_header[i].filetype_header);
        file_header[i].p_pcre = pcre_compile(row[1]  , 0, &error, &erroffset, NULL);///YS 编译正则式模板
        i++;
    }
    mysql_free_result(res);

    return 0;
}


/*********************
获取用户的流量
参数1，SQL语句
参数2，已连接的SQL句柄
**********************/
int sql_get_flow(char *SELECT_QUERY,MYSQL *sock)
{
    MYSQL_ROW row;
    MYSQL_RES *res;
    int ip1,ip2,ip3,ip4;
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
    //ser_IP,Flow,Flow_limit
    while ((row = mysql_fetch_row(res)))
    {
        if(row[0])
        {
            sscanf(row[0],"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);///YS 获取IP地址的最后一位
            sprintf(userf[ip4].ip,"%s",row[0]);
            userf[ip4].sub_net=ip3;///YS 获取IP地址的倒数第二位，以确定子网
        }

        if(row[1])  userf[ip4].flow=atol(row[1]);
        else userf[ip4].flow=0;
        if(row[2]) userf[ip4].limit=atol(row[2]);
        else userf[ip4].limit=-1;
        dbug("ip:%d  %ld  \n",ip4 ,userf[ip4].flow);
    }

    mysql_free_result(res);
    return 0;
}

/*********************
获取要过滤的文件类型（.JPG  .RAR .ZIP）
参数1，SQL语句
参数2，已连接的SQL句柄
**********************/
int sql_get_file_name(char *SELECT_QUERY,MYSQL *sock)
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
    strcat(filename_type,"filetype:");
    //char name_temp[128];
    while ((row = mysql_fetch_row(res)))
    {
        strcat(filename_type,row[0]);///YS 这个filename_type，是为了后面判断文件的类型是不是该被保存
    }
    dbug("%s  \n",filename_type);
    mysql_free_result(res);
    return 0;
}


/*********************
该函数已经不再使用了
**********************/
int sql_get_feature_to_pcre2(char *SELECT_QUERY,MYSQL *sock)
{
    MYSQL_ROW row;
    MYSQL_RES *res;
    const char *error;
    int erroffset;
    int i=0;
    dbug("sql get serverip to hash  list\n");

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
    //char name_temp[128];
    while ((row = mysql_fetch_row(res)))//ys ready for
    {
        dbug("%s>>>>>>>>>>>>>>",row[0]);
        sprintf(match_file_type[i],"%s",row[0]);
        sprintf(s_pcre[i].name,row[0]);
        sprintf(s_pcre[i].feature,row[1]);
        s_pcre[i].p_pcre = pcre_compile(row[1]  , 0, &error, &erroffset, NULL);
        dbug("%s \n",s_pcre[i].feature);//YY
        i++;
    }
    file_type_sum=i;
    return 0;
}


/***************************
*从数据库里获取用户到哈希表中
*参数：SELECT_QUERY，是查询数据库的SQL（看你要查的是哪个类别的URL）
***************************/
int sql_get_user_to_hash(char *SELECT_QUERY,MYSQL *sock,GHashTable * hash_table)
{
    MYSQL_ROW row;
    MYSQL_RES *res;
    MYSQL_RES *res2;

    if(mysql_query(sock,SELECT_QUERY))
    {
        //dbug(SELECT_QUERY);
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
    mysql_free_result(res2);
    return 0;   //
}


void mysql_end(MYSQL_RES *res,MYSQL *sock)
{
    mysql_free_result(res);
    mysql_close(sock);
}
