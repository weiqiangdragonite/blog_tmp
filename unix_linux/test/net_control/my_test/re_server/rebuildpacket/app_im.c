///YS 文件里大量使用了内存查找函数memmem，如：我们要查找aaa*****bbb中，*号里的内容时，我们先找到aaa，再找bbb结尾的文件，这样就能确定我们要的内容了
///YS XXX_XX_S,这个S是start,对应的就是aaa；XXX_XX_E这是end,对应的就bbb，这是我们在数据库里保存的值
///YS XXX_XX_S2，这个S2是start2因为有的协议的同一个字段会有多个特征，这是第二个特征
#include <stdio.h>
#include "time.h"
#include "packet_info.h"
#include "config.h"
#include <mysql/mysql.h>
#include <string.h>
#include <unistd.h>

extern struct pcre_info_im app_im_pcre[pcre_num];///YS 聊天协议审计的特征字结构体
extern char  net_addr [8];///YS 子网，192.168

/*********************************
审计聊天内容函数
参数1，我们自定义的数据结构包
**********************************/
int process_im( struct packet_info * packet_tmp)
{

    int i=0,len_start,len_end;
    char *p;
    int flag=0;///YS 1是发送信息，2是接收信息
    char content[1220]={0};
    char receiver[1220]={0};
    char login_user[1220]={0};
    char sender[1220]={0};

    for(i=0; i<pcre_num; i++)///YS 我们现在可以处理的协议数量
    {
        if(strlen(app_im_pcre[i].name)<1) break;///YS 如果协议名长度小于1，就说明已经轮了一圈了



        ///YS 先找一下有没有用户LOGIN
        if((p=(char *)memmem(packet_tmp->raw_packet,packet_tmp->packet_size,app_im_pcre[i].login_user_s,strlen(app_im_pcre[i].login_user_s))))
        {
            len_start=p-(char *)packet_tmp->raw_packet;
            if((p=memmem(packet_tmp->raw_packet+len_start,packet_tmp->packet_size-len_start,app_im_pcre[i].login_user_e,strlen(app_im_pcre[i].login_user_e))))
            {
                len_end=p-(char *)packet_tmp->raw_packet;
                if(memmem(packet_tmp->raw_packet,packet_tmp->packet_size,app_im_pcre[i].login_user_ex,strlen(app_im_pcre[i].login_user_ex)))
                {
                    ///YS 找到用户名了LOGIN
                    if(len_end-len_start-strlen(app_im_pcre[i].sender_s)+1>sizeof(login_user))///YS 判断长度后再进行字符串赋值，不然会越界出错
                    {
                        snprintf(login_user,sizeof(login_user),"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].login_user_s));
                    }
                    else
                    {
                        snprintf(login_user,len_end-len_start-strlen(app_im_pcre[i].login_user_s)+1,"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].login_user_s));
                    }
                }
            }
        }


        ///YS 找一下有没有聊天的内容
        if((p=(char *)memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].content_feature_s,strlen(app_im_pcre[i].content_feature_s))))
        {
            len_start=p-(char *)packet_tmp->raw_packet;

            if(!strcmp(app_im_pcre[i].content_feature_e,"end"))///YS 如果聊天内容的结束标记 是数据包的结尾，结尾我们用"END“表示
            {
                if(memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].content_feature_ex,strlen(app_im_pcre[i].content_feature_ex)))
                {
                    ///YS 找到聊天的内容
                    if(packet_tmp->cap_size-len_start-strlen(app_im_pcre[i].content_feature_s)+1>sizeof(content))
                    {
                        snprintf(content,sizeof(content),"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].content_feature_s));
                    }
                    else
                    {
                        snprintf(content,packet_tmp->cap_size-len_start-strlen(app_im_pcre[i].content_feature_s)+1,"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].content_feature_s));
                    }
                }

            }
            else///YS 如果聊天内容的结束标记不是数据包的结尾
            {
                if((p=memmem(packet_tmp->raw_packet+len_start,packet_tmp->cap_size-len_start,app_im_pcre[i].content_feature_e,strlen(app_im_pcre[i].content_feature_e))))
                {
                    len_end=p-(char *)packet_tmp->raw_packet;

                    if(memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].content_feature_ex,strlen(app_im_pcre[i].content_feature_ex)))
                    {
                        ///YS 找到聊天的内容
                        if(len_end-len_start-strlen(app_im_pcre[i].content_feature_s)+1>sizeof(content))
                        {
                            snprintf(content,sizeof(content),"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].content_feature_s));
                        }
                        else
                        {
                            snprintf(content,len_end-len_start-strlen(app_im_pcre[i].content_feature_s)+1,"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].content_feature_s));
                        }
                    }
                }
            }

        }

        ///YS 找一下有没有收件人
        if((p=(char *)memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].receiver_s,strlen(app_im_pcre[i].receiver_s))))
        {
            len_start=p-(char *)packet_tmp->raw_packet;
            if((p=memmem(packet_tmp->raw_packet+len_start,packet_tmp->cap_size-len_start,app_im_pcre[i].receiver_e,strlen(app_im_pcre[i].receiver_e))))
            {
                len_end=p-(char *)packet_tmp->raw_packet;
                if(memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].receiver_ex,strlen(app_im_pcre[i].receiver_ex)))
                {
                    flag=1;
                    ///YS 找到收件人
                    if(len_end-len_start-strlen(app_im_pcre[i].receiver_s)+1>sizeof(receiver))
                    {
                        snprintf(receiver,sizeof(receiver),"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].receiver_s));
                    }
                    else
                    {
                        snprintf(receiver,len_end-len_start-strlen(app_im_pcre[i].receiver_s)+1,"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].receiver_s));
                    }
                }
            }
        }


        ///YS 找一下有没有发件人
        if((p=(char *)memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].sender_s,strlen(app_im_pcre[i].sender_s))))
        {
            len_start=p-(char *)packet_tmp->raw_packet;
            if((p=memmem(packet_tmp->raw_packet+len_start,packet_tmp->cap_size-len_start,app_im_pcre[i].sender_e,strlen(app_im_pcre[i].sender_e))))
            {
                len_end=p-(char *)packet_tmp->raw_packet;
                if(memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].sender_ex,strlen(app_im_pcre[i].sender_ex)))
                {
                    flag=1;
                    ///YS 找到发件人
                    if(len_end-len_start-strlen(app_im_pcre[i].sender_s)+1>sizeof(sender))
                    {
                        snprintf(sender,sizeof(sender),"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].sender_s));
                    }
                    else
                    {
                        snprintf(sender,len_end-len_start-strlen(app_im_pcre[i].sender_s)+1,"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].sender_s));
                    }
                }
            }
        }



       /*****************上面的都是发出去的，下面的这几个是人家发过来的******************************************/
        ///YS 找一下有没有发件人
        if((p=(char *)memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].in_sender_s,strlen(app_im_pcre[i].in_sender_s))))
        {
            len_start=p-(char *)packet_tmp->raw_packet;
            if((p=memmem(packet_tmp->raw_packet+len_start,packet_tmp->cap_size-len_start,app_im_pcre[i].in_sender_e,strlen(app_im_pcre[i].in_sender_e))))
            {
                len_end=p-(char *)packet_tmp->raw_packet;
                if(memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].in_ex,strlen(app_im_pcre[i].in_ex)))
                {

                    ///YS 找到发件人
                    if(len_end-len_start-strlen(app_im_pcre[i].in_sender_s)+1>sizeof(sender))
                    {
                        snprintf(sender,sizeof(sender),"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].in_sender_s));
                    }
                    else
                    {
                        snprintf(sender,len_end-len_start-strlen(app_im_pcre[i].in_sender_s)+1,"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].in_sender_s));
                    }
                    flag=2;
                }
            }
        }
        ///YS 找一下有没有内容
        if((p=(char *)memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].in_content_feature_s,strlen(app_im_pcre[i].in_content_feature_s))))
        {
            len_start=p-(char *)packet_tmp->raw_packet;

            if(!strcmp(app_im_pcre[i].in_content_feature_e,"end"))
            {
                if(memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].in_ex,strlen(app_im_pcre[i].in_ex)))
                {
                    ///YS 找到聊天的内容
                    if(packet_tmp->cap_size-len_start-strlen(app_im_pcre[i].in_content_feature_s)+1>sizeof(content))
                    {
                        snprintf(content,sizeof(content),"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].in_content_feature_s));
                    }
                    else
                    {
                        snprintf(content,packet_tmp->cap_size-len_start-strlen(app_im_pcre[i].in_content_feature_s)+1,"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].in_content_feature_s));
                    }
                }

            }
            else
            {
                if((p=memmem(packet_tmp->raw_packet+len_start,packet_tmp->cap_size-len_start,app_im_pcre[i].in_content_feature_e,strlen(app_im_pcre[i].in_content_feature_e))))
                {
                    len_end=p-(char *)packet_tmp->raw_packet;

                    if(memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].in_ex,strlen(app_im_pcre[i].in_ex)))
                    {
                        ///YS 找到聊天的内容
                        if(len_end-len_start-strlen(app_im_pcre[i].in_content_feature_s)+1>sizeof(content))
                        {
                            snprintf(content,sizeof(content),"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].in_content_feature_s));
                        }
                        else
                        {
                            snprintf(content,len_end-len_start-strlen(app_im_pcre[i].in_content_feature_s)+1,"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].in_content_feature_s));
                        }
                    }
                }
            }

        }


        ///YS 找一下有没有收件人
        if((p=(char *)memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].in_receiver_s,strlen(app_im_pcre[i].in_receiver_s))))
        {
            len_start=p-(char *)packet_tmp->raw_packet;
            if((p=memmem(packet_tmp->raw_packet+len_start,packet_tmp->cap_size-len_start,app_im_pcre[i].in_receiver_e,strlen(app_im_pcre[i].in_receiver_e))))
            {
                len_end=p-(char *)packet_tmp->raw_packet;
                if(memmem(packet_tmp->raw_packet,packet_tmp->cap_size,app_im_pcre[i].in_ex,strlen(app_im_pcre[i].in_ex)))
                {

                    flag=2;
                    ///YS 找到收件人
                    if(len_end-len_start-strlen(app_im_pcre[i].in_receiver_s)+1>sizeof(receiver))
                    {
                        snprintf(receiver,sizeof(receiver),"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].in_receiver_s));
                    }
                    else
                    {
                        snprintf(receiver,len_end-len_start-strlen(app_im_pcre[i].in_receiver_s)+1,"%s",packet_tmp->raw_packet+len_start+strlen(app_im_pcre[i].in_receiver_s));
                    }
                }
            }
        }

		///YS 找一下有没EX字段，如果有该内容存在，我们就排除这个包
    if(strlen(app_im_pcre[i].ex_1))
       if((memmem(content,strlen(content),app_im_pcre[i].ex_1,strlen(app_im_pcre[i].ex_1))))
        {
            return 0;
        }
				/****************************下面是写入数据库****************************************************/
        if(strlen(login_user)>2)///YS 这是用户登录，写入数据库
        {
            dbug("login_user>>>>>>>>>>   %s\n",login_user);
            if(strstr(packet_tmp->src_ip,net_addr))
            {
                insert_into_db_im("NULL",login_user,packet_tmp->src_ip,app_im_pcre[i].name,"NULL","用户登陆");
                return 0;
            }
        }
        else
        {

            if(strlen(content)>2&&flag==1)///YS FLAG=1是发送信息
            {
                if(strlen(receiver)<=0) sprintf(receiver,"%s","未知");///YS 这是因为有的协议里面找不到这个值，因为他们用了ID，而不用名字等等
                if(strstr(packet_tmp->src_ip,net_addr))
                {
                    insert_into_db_im(receiver,content,packet_tmp->src_ip,app_im_pcre[i].name, sender ,"发送信息");

                }
                else
                {
                    insert_into_db_im(receiver,content,packet_tmp->dst_ip,app_im_pcre[i].name, sender ,"发送信息");

                }

                return 0;
            }
        }
        if(strlen(sender)&&strlen(content)>2&&flag==2)///YS FLAG=2接收信息
        {

            if(strstr(packet_tmp->src_ip,net_addr))
            {
                insert_into_db_im(receiver,content,packet_tmp->src_ip,app_im_pcre[i].name, sender ,"接收信息");
            }
            else
            {
                insert_into_db_im(receiver,content,packet_tmp->dst_ip,app_im_pcre[i].name, sender ,"接收信息");
            }

            return 0;
        }

    }



    return 0;
}







/*
int process_im( char * buff,long int buff_len)
{
    ///find the pcre
    int i=0,rcCM,len_start,len_end;
    char *p;
    char sender[256];
    char receiver[256];
    char content[1024*256];
    int ovector[OVECCOUNT];
    dbug("process im \n\n");
    for(i=0; i<pcre_num; i++)
    {
        if(strlen(app_im_pcre[i].name)<1) return 0;
        dbug("process im %s  %s\n\n",app_im_pcre[i].name,app_im_pcre[i].feature);
        rcCM = pcre_exec(app_im_pcre[i].p_pcre, NULL, buff, buff_len, 0, 0, ovector, OVECCOUNT);  //匹配pcre编译好的模式，成功返回正数，失败返回负数
        if (rcCM==PCRE_ERROR_NOMATCH )
        {
            dbug("no match !!!!\n");
            return 0;
        }
        if(rcCM>=0)
        {
            //dbug("match  it  content start   %d!!! %x |%x |%x |%x \n",ovector[0],buff[ovector[0]],buff[ovector[0]+1],buff[ovector[0]+2],buff[ovector[0]+3]);//start of content;
            dbug("match it im name is :  \"  %s  \"  \n",app_im_pcre[i].name);

            {
                ///get sender name
                p=(char *)memmem( buff,buff_len,app_im_pcre[i].sender_s,strlen(app_im_pcre[i].sender_s));
                if(p)
                {
                    len_start=p-buff;
                    p=(char *)memmem( buff+len_start+strlen(app_im_pcre[i].sender_s),buff_len,app_im_pcre[i].sender_e,strlen(app_im_pcre[i].sender_e) );
                    if(p)
                    {
                        len_end=p-buff;

                        if(len_end-len_start-strlen(app_im_pcre[i].sender_s)+1>sizeof(sender))
                        {
                            snprintf(sender,sizeof(sender),"%s",buff+len_start+strlen(app_im_pcre[i].sender_s));
                        }
                        else
                        {
                            snprintf(sender,len_end-len_start-strlen(app_im_pcre[i].sender_s)+1,"%s",buff+len_start+strlen(app_im_pcre[i].sender_s));
                        }
                    }
                }
                else
                {
                    if(strlen(app_im_pcre[i].sender_s_2))
                    {
                        p=(char *)memmem( buff,buff_len,app_im_pcre[i].sender_s_2,strlen(app_im_pcre[i].sender_s_2));
                        if(p)
                        {
                            len_start=p-buff;
                            p=(char *)memmem( buff+len_start+strlen(app_im_pcre[i].sender_s_2),buff_len,app_im_pcre[i].sender_e_2,strlen(app_im_pcre[i].sender_e_2) );
                            if(p)
                            {
                                len_end=p-buff;

                                if(len_end-len_start-strlen(app_im_pcre[i].sender_s_2)+1>sizeof(sender))
                                {
                                    snprintf(sender,sizeof(sender),"%s",buff+len_start+strlen(app_im_pcre[i].sender_s_2));
                                }
                                else
                                {
                                    snprintf(sender,len_end-len_start-strlen(app_im_pcre[i].sender_s_2)+1,"%s",buff+len_start+strlen(app_im_pcre[i].sender_s_2));
                                }

                            }
                        }
                    }
                }


                ///get receiver name
                p=(char *)memmem( buff,buff_len,app_im_pcre[i].receiver_s,strlen(app_im_pcre[i].receiver_s));
                if(p)
                {
                    len_start=p-buff;
                    p=(char *)memmem( buff+len_start+strlen(app_im_pcre[i].receiver_s),buff_len,app_im_pcre[i].receiver_e,strlen(app_im_pcre[i].receiver_e) );
                    if(p)
                    {
                        len_end=p-buff;
                        if(len_end-len_start-strlen(app_im_pcre[i].receiver_s)+1>sizeof(receiver))
                        {
                            snprintf(receiver,sizeof(receiver),"%s",buff+len_start+strlen(app_im_pcre[i].receiver_s));
                        }
                        else
                        {
                            snprintf(receiver,len_end-len_start-strlen(app_im_pcre[i].receiver_s)+1,"%s",buff+len_start+strlen(app_im_pcre[i].receiver_s));
                        }
                    }
                }
                else
                {
                    if(strlen(app_im_pcre[i].receiver_s_2))
                    {
                        p=(char *)memmem( buff,buff_len,app_im_pcre[i].receiver_s_2,strlen(app_im_pcre[i].receiver_s_2));
                        if(p)
                        {
                            len_start=p-buff;
                            p=(char *)memmem( buff+len_start+strlen(app_im_pcre[i].receiver_s_2),buff_len,app_im_pcre[i].receiver_e_2,strlen(app_im_pcre[i].receiver_e_2) );
                            if(p)
                            {
                                len_end=p-buff;

                                if(len_end-len_start-strlen(app_im_pcre[i].receiver_s_2)+1>sizeof(receiver))
                                {
                                    snprintf(receiver,sizeof(receiver),"%s",buff+len_start+strlen(app_im_pcre[i].receiver_s_2));
                                }
                                else
                                {
                                    snprintf(receiver,len_end-len_start-strlen(app_im_pcre[i].receiver_s_2)+1,"%s",buff+len_start+strlen(app_im_pcre[i].receiver_s_2));
                                }

                            }
                        }
                    }
                }


                ///get content
                p=(char *)memmem( buff,buff_len,app_im_pcre[i].content_feature_s,strlen(app_im_pcre[i].content_feature_s));
                if(p)
                {
                    len_start=p-buff;
                    if(!strcmp("end",app_im_pcre[i].content_feature_e))
                    {
                        snprintf(content,sizeof(content),"%s",buff+len_start+strlen(app_im_pcre[i].content_feature_s));
                    }
                    else
                    {
                        p=(char *)memmem( buff+len_start+strlen(app_im_pcre[i].content_feature_s),buff_len,app_im_pcre[i].content_feature_e,strlen(app_im_pcre[i].content_feature_e) );
                        if(p)
                        {
                            len_end=p-buff;
                            if(len_end-len_start-strlen(app_im_pcre[i].content_feature_s)+1>sizeof(content))
                            {
                                snprintf(content,sizeof(content),"%s",buff+len_start+strlen(app_im_pcre[i].content_feature_s));
                            }
                            else
                            {
                                snprintf(content,len_end-len_start-strlen(app_im_pcre[i].content_feature_s)+1,"%s",buff+len_start+strlen(app_im_pcre[i].content_feature_s));
                            }
                        }
                    }
                }
                else
                {
                    if(strlen(app_im_pcre[i].content_feature_s_2))
                    {
                        p=(char *)memmem( buff,buff_len,app_im_pcre[i].content_feature_s_2,strlen(app_im_pcre[i].content_feature_s_2));
                        if(p)
                        {
                            len_start=p-buff;
                            if(!strcmp("end",app_im_pcre[i].content_feature_e_2))
                            {
                               snprintf(content,sizeof(content),"%s",buff+len_start+strlen(app_im_pcre[i].content_feature_s_2));
                            }
                            else
                            {
                                p=(char *)memmem( buff+len_start+strlen(app_im_pcre[i].content_feature_s_2),buff_len,app_im_pcre[i].content_feature_e_2,strlen(app_im_pcre[i].content_feature_e_2) );
                                if(p)
                                {
                                    len_end=p-buff;
                                    snprintf(content,len_end-len_start-strlen(app_im_pcre[i].content_feature_s_2)+1,"%s",buff+len_start+strlen(app_im_pcre[i].content_feature_s_2));

                                    if(len_end-len_start-strlen(app_im_pcre[i].content_feature_s)+1>sizeof(content))
                                    {
                                        snprintf(content,sizeof(content),"%s",buff+len_start+strlen(app_im_pcre[i].content_feature_s_2));
                                    }
                                    else
                                    {
                                        snprintf(content,len_end-len_start-strlen(app_im_pcre[i].content_feature_s_2)+1,"%s",buff+len_start+strlen(app_im_pcre[i].content_feature_s_2));
                                    }
                                }
                            }
                        }
                    }
                }
            }



dbug("get the im message !!! %s   ||  %s  ||  %s\n",sender,receiver,content);
            return 0;

        }

    }
    return 0;
}
*/
