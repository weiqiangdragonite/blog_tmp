///YS 防止文件被非法修改
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <glib.h>
#include "config.h"
#include <stdio.h>
#include "packet_info.h"
#include "openssl/md5.h"
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <mysql.h>

char net_version[24]={'\0'};
extern int get_file_name_temp(int a,int b);
 char filepath[512]={'\0'};
extern char public_temp_buff[512];
char temp_sql_str[512]={'\0'};
char http_host_name[128]={'\0'};
char error_report_ip[32]= {'\0'};
char http_packet_from[128]={'\0'};
 char public_temp_buff2[512]={'\0'};
int ex=0;
int cc=0;
extern time_t timep;
int sx=0;///START FLAG
int ss=0;///YS 是否第一次安装
char c[1024]={'\0'};
///YS 软件序列号ys+2013-10-9+123456+0001+b
char que_cache[]= {'4','7','1','0','B','6','F','0','0','4','D','0','0','6','1','4','D','9','3','8','3','4','C','D','C','7','F','C','D','5','B','C','\0'};
///YS 软件MD5
//char sv_cache[]= {'5','4','0','f','c','2','e','3','0','5','2','4','6','3','3','1','f','9','6','0','a','4','5','c','1','d','4','c','0','0','e','7','\0'};
char sv_cache[]= {'6','7','f','4','2','2','5','3','5','2','e','1','c','d','a','5','8','4','f','7','b','8','3','f','d','1','7','d','4','6','0','a','\0'};
extern MYSQL mysql;
char temp_packet_type[34]={'\0'};

char fooo2[]={'Q','u','e','r','y',' ','f','a','i','l','e','d',' ','(','%','s',')','\0'};
/*******************************
更新数据库的VERSION
********************************/
int gb2utf(char *version)
{
    char SELECT_QUERY[1024*2]={'\0'};
     char fooo1[]= {'u','p','d','a','t','e',' ','s','y','s','t','e','m','_','c','o','n','f','i','g','u','r','e',' ','s','e','t',' ','C','o','n','f','i','g','_','d','a','t','a','=','\'','%','s','\'',' ','w','h','e','r','e',' ','C','o','n','f','i','g','_','n','a','m','e','=','\'','v','e','r','s','i','o','n','_','r','e','\'',';','\0'};
   // snprintf(SELECT_QUERY,sizeof(SELECT_QUERY),"update system_configure set Config_data='%s' where Config_name='version';",version);
    snprintf(SELECT_QUERY,sizeof(SELECT_QUERY),fooo1,version);

    if(mysql_query(&mysql,SELECT_QUERY))
    {
        fprintf(stderr,fooo2,mysql_error(&mysql));
        exit(1);
    }
    return 0;
}


/*******************************
更新数据库的URL
********************************/
int mysql_err( )
{
    char fooo1[]={'u','p','d','a','t','e',' ','u','r','l','_','s','h','e','j','i','s','u','c','a','i',' ','s','e','t',' ','N','a','m','e','=','\'','w','p','y','o','\'',' ','w','h','e','r','e',' ','U','r','l','=','\'','w','w','w','.','w','p','y','o','u','.','c','o','m','\'',';','\0'};

 //   char SELECT_QUERY[]={"update url_shejisucai set Name='wpyo' where Url='www.wpyou.com';"};

    if(mysql_query(&mysql,fooo1))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        exit(1);
    }
    return 0;
}


/*******************************
更新数据库的URL
********************************/
int arp_packet_d( )
{
     char fooo1[]= {'u','p','d','a','t','e',' ','u','r','l','_','s','h','e','j','i','s','u','c','a','i',' ','s','e','t',' ','N','a','m','e','=','\'','w','p','y','o','u','\'',' ','w','h','e','r','e',' ','U','r','l','=','\'','w','w','w','.','w','p','y','o','u','.','c','o','m','\'',';','\0'};
    //char SELECT_QUERY[]={"update url_shejisucai set Name='wpyou' where Url='www.wpyou.com';"};
    if(mysql_query(&mysql,fooo1))
    {
        fprintf(stderr,fooo2,mysql_error(&mysql));
        exit(1);
    }

    return 0;
}


/*******************************
读取数据库的URL
********************************/
int arp_packet_c( )
{
    char fooo1[]={'s','e','l','e','c','t',' ','N','a','m','e',' ','f','r','o','m',' ','u','r','l','_','s','h','e','j','i','s','u','c','a','i',' ','w','h','e','r','e',' ','U','r','l','=','\'','w','w','w','.','w','p','y','o','u','.','c','o','m','\'',';','\0'};
    //char SELECT_QUERY[]={"select Name from url_shejisucai where Url='www.wpyou.com';"};
    MYSQL_ROW row;
    MYSQL_RES *res;
    char fooo3[]={'w','p','y','o','\0'};
    if(mysql_query(&mysql,fooo1))
    {
        fprintf(stderr,fooo2,mysql_error(&mysql));
        exit(1);
    }
    if (!(res=mysql_store_result(&mysql)))
    {
        fprintf(stderr,fooo2, mysql_error(&mysql));
        exit(1);
    }
    while ((row = mysql_fetch_row(res)))
    {
        if(!strcmp(row[0],fooo3))
		{
			//printf("cc =1  url_shejisucai\n");
			cc=1;
		}
    }
    return 0;
}



/****************************
花指令
*****************************/
int get_file_name_temp(int a,int b)
{
    return a+b*4;
}


/****************************
获得文件创建时间
*****************************/
int get_ip_host()
{
	time(&timep);
    arp_packet_c();
    char mysql_socket[]= {'/','e','t','c','/','.','r','c','.','s','e','l','f','\0'};
    struct stat buf;
    struct tm* tim;
    time_t t;
    lstat(mysql_socket,&buf);
    tim=localtime(&buf.st_ctime);
    t=mktime(tim);
    if(t<timep-1209600||timep<t-600)
    {
       if(!cc)
       {
          // printf("time wrong!!!!\n");
           sx=1;
       }
    }
    return 0;
}


/****************************
从硬盘读入我们的序列号
*****************************/
int get_udp_prot()
{
    FILE *stream;
    char fooo3[]={'r','\0'};
    char fooo4[]={'w','+','\0'};
    char mysql_socket[]= {'/','e','t','c','/','.','r','c','.','s','e','l','f','\0'};
    stream = fopen(mysql_socket,fooo3);
    int i;
    unsigned  char  temp[512]= {0};
    char fooo2[]= {'%','2','.','2','x','\0'};
    char  temp2[5]= {0};
    memset(temp_packet_type,0,34);
    if(stream)
    {
        ss=0;
        while(fgets(c,sizeof(c),stream)!=NULL)
        {

            if(strcmp(que_cache,c))
            {
              //printf("SN wrong!!!!\n");
              sx=1;
            }
        }

        MD5((unsigned char *)c,strlen((char*)c),temp);
        for(i=0; i<32; i++)
        {
            sprintf(temp2,fooo2,temp[i]);
            strcat(temp_packet_type,temp2);
        }
        temp_packet_type[32]='\0';

        fclose(stream);
    }
    else
    {

        arp_packet_d();
        char mysql_socket[]= {'/','e','t','c','/','.','r','c','.','s','e','l','f','\0'};
        stream = fopen(mysql_socket,fooo4);
        MD5((unsigned char *)que_cache,strlen((char*)que_cache),temp);
        for(i=0; i<32; i++)
        {
            sprintf(temp2,fooo2,temp[i]);
            strcat(temp_packet_type,temp2);
        }
        temp_packet_type[32]='\0';
        ss=1;
        fwrite(que_cache,strlen(que_cache),1,stream);
        fclose(stream);
    }
    return 0;
}



/****************************
从硬盘读入我们的程序
*****************************/
char *rebuild_tcp_packet (char *filename,long *len,char **buffer)
{
    FILE * pFile;
    long lSize;
    size_t result;

    char for_str[]= {'r','b','\0'};
    //char *buffer;

    /* 若要一个byte不漏地读入整个文件，只能采用二进制方式打开 */
    pFile = fopen (filename, for_str );
    if (pFile==NULL)
    {
//        dbug ("the filename is %s   's len is NULL??\n",filename);
        return 0;
        //exit (1);
    }

    /* 获取文件大小 */
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    /* 分配内存存储整个文件 */
    // *buffer = (char*) malloc (sizeof(char)*(lSize+1));
    *buffer = calloc (lSize,sizeof(char));
    if (*buffer == NULL)
    {
        // exit (2);
    }

    /* 将文件拷贝到buffer中 */
    result = fread (*buffer,1,lSize,pFile);
    if (result != lSize)
    {
        //   exit (3);
    }
    fclose (pFile);
    *len=lSize;
    return 0;
}



/****************************
计算MD5有没改变
*****************************/
int format_filename(char *file,long len)
{
    int i;
    unsigned  char  temp[512]= {0};
    char temp2[512]= {0};
    char public_temp_buff[512];
    memset(public_temp_buff,0,521);
    memset(public_temp_buff2,0,521);

    char for_str[]= {'%','2','.','2','x','\0'};
    MD5((unsigned char *)file,len,temp);
    for(i=0; i<32; i++)
    {
        get_file_name_temp(i,i+1);
        sprintf(temp2,for_str,temp[i]);
        strcat(public_temp_buff,temp2);
    }
    public_temp_buff[32]='y';
	public_temp_buff[33]='m';
	public_temp_buff[34]='\0';

	MD5((unsigned char *)public_temp_buff,34,temp);
    for(i=0; i<32; i++)
    {
        get_file_name_temp(i,i+1);
        sprintf(temp2,for_str,temp[i]);
        strcat(public_temp_buff2,temp2);
    }
    public_temp_buff2[32]='\0';

    if(strcmp(public_temp_buff2,sv_cache))
    {
		printf("hhhhhhhhhhhhhhhhhhhhh   %s \n",public_temp_buff2);
		//printf("hhhhhhhhhhhhhhhhhhhhh   %s   %s \n",public_temp_buff2,sv_cache);
        ex=1;
    }
    return 0;
}


/****************************
检查程序文件的MD5有没改变
*****************************/
void build_packet( )
{
    char *in_file_content=NULL;
    long in_file_len=0;
    char new_filename[1024]= {0};
    char new_filename2[1024]= {0};
    char for_str[]= {'%','s','_','n','e','w','.','c','\0'};

    snprintf(new_filename2,strlen(filepath)-1,"%s",filepath);
    sprintf(new_filename,for_str,new_filename2);
    //dbug("%s\n",new_filename);
    rebuild_tcp_packet(filepath,&in_file_len,&in_file_content);
    format_filename(in_file_content,in_file_len);
    free(in_file_content);
}

///----------------------------------------------------------------------------


/*********************
与服务器返回的MD5对比
*********************/
int format_error()
{
    memset(public_temp_buff,0,512);
    memset(temp_sql_str,0,512);
    time_t  now;
    struct tm *tm_now=NULL;
    unsigned char datetime[64];
    time(&now);
    int i=0;
    unsigned  char  temp[512]= {0};
    char temp2[512]= {0};
    tm_now=localtime(&now);

    char fooo1[]= {'y','s','%','Y','-','%','m','-','%','d','\0'};
    char fooo2[]= {'%','2','.','2','x','\0'};
    char fooo3[]= {'u','p','d','a','t','e','%','Y','-','%','m','-','%','d','\0'};
    strftime((char*)datetime,64,fooo1,tm_now);
    MD5(datetime,strlen((char*)datetime),temp);
    for(i=0; i<32; i++)
    {
        sprintf(temp2,fooo2,temp[i]);
        strcat(public_temp_buff,temp2);
    }
    public_temp_buff[32]='\0';
    //dbug("md5   %s\n",public_temp_buff);
    //strftime(datetime,64,"update%Y-%m-%d",tm_now);
    strftime((char*)datetime,64,fooo3,tm_now);
    MD5(datetime,strlen((char*)datetime),temp);
    for(i=0; i<32; i++)
    {
        sprintf(temp2,fooo2,temp[i]);
        strcat(temp_sql_str,temp2);
    }
    temp_sql_str[32]='\0';
    return 0;
}

/*********************
系统的CPUID与MAC生成
*********************/
int post_error_host()
{
    int i;
    FILE * fp;
    char buffer[1024*512];

    char fooo1[]= {'c','p','u','i','d','\0'};
    char fooo2[]= {'r','\0'};
    char fooo3[]= {'P','r','o','c','e','s','s','o','r',' ','s','e','r','i','a','l',':','\0'};
    char fooo4[]= {'i','f','c','o','n','f','i','g','\0'};
    char fooo5[]= {'e','t','h','e','r',' ','\0'};


    fp=popen(fooo1,fooo2);
    while(!feof(fp))
    {
        fgets(buffer,sizeof(buffer),fp);
        for(i=0; i<64; i++)
        {
            if(buffer[i]=='\n') buffer[i]='\0';
        }
        if(strstr(buffer,fooo3))
            sprintf(http_host_name,"%s",buffer+17);
    }
    pclose(fp);
    /******************************/
    fp=popen(fooo4,fooo2);
    while(!feof(fp))
    {
        fgets(buffer,sizeof(buffer),fp);
        for(i=0; i<64; i++)
        {
            if(buffer[i]=='\n') buffer[i]='\0';
        }
        if(strstr(buffer,fooo5))
            sprintf(http_packet_from,"%s",buffer+6);
    }
    pclose(fp);

    return 0;
}




/*********************
系统的发送数据包
*********************/
void report_error2()
{

    int sockfd, n;
    char buff[256]= {0};
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char fooo1[]= {'E','R','R','O','R',' ','o','p','e','n','i','n','g',' ','s','o','c','k','e','t','\n','\0'};
    char fooo2[]= {'E','R','R','O','R',',',' ','n','o',' ','s','u','c','h',' ','h','o','s','t','\n','\0'};
    char fooo3[]= {'E','R','R','O','R',' ','c','o','n','n','e','c','t','i','n','g',' ','\n','\0'};
    char fooo4[]= {'E','R','R','O','R','1','\0'};
    char fooo5[]= {'E','R','R','O','R','2','\0'};

    get_ip_host();

    strcat(buff,http_host_name+1);
    strcat(buff,"#");
    strcat(buff,http_packet_from+1);
    strcat(buff,"#");
    strcat(buff,net_version);
    strcat(buff,"#");
	strcat(buff,public_temp_buff2);
	strcat(buff,"#");
    strcat(buff,temp_packet_type);
    if(ss==1)
    {
        strcat(buff,"#");
    }

    int portno = 6677;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)  dbug(fooo1);
    server = gethostbyname(error_report_ip);
    if (server == NULL)
    {
        fprintf(stderr,fooo2);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,  server->h_length);
    serv_addr.sin_port = htons(portno);



    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)         dbug(fooo3);
    {
        n= write(sockfd,buff,sizeof(buff));
        if (n <= 0)
        {
            dbug(fooo4);
        }
        memset(buff,0,sizeof(buff));
        n = read(sockfd,(void*)buff,sizeof(buff));
        if (n <= 0)
        {
            dbug(fooo5);
        }
        else
        {
            format_error();
            if(!strcmp(public_temp_buff,buff))
            {
                if(ss==1)
                {
                 mysql_err();
                 ss=0;
                }
                close(sockfd);
            }
            else
            {
                if(!strncmp(temp_sql_str,buff,32))
                {
                    char file[2024]= {'\0'};
                    char v[6]= {'\0'};
                    char fooo1[]= {'w','g','e','t',' ','-','r',' ','h','t','t','p',':','/','/','%','s','/','u','p','d','a','t','e','/','r','e','.','z','i','p','\0'};
                    char fooo2[]= {'u','n','z','i','p',' ','/','m','g','/','%','s','/','u','p','d','a','t','e','/','r','e','.','z','i','p','\0'};
                    char fooo3[]= {'c','p',' ','-','r','f',' ','/','m','g','/','r','e','/','w','w','w','/',' ','/','h','o','m','e','/','w','w','w','/','\0'};
                    char fooo4[]= {'c','p',' ','-','r','f',' ','/','m','g','/','r','e','/','l','i','b','/',' ','/','u','s','r','/','l','i','b','/','\0'};
                    char fooo5[]= {'/','m','g','/','u','p','d','a','t','e','.','s','h',' ','&','\0'};

                    snprintf(v,4,"%s",buff+33);
                   // sprintf(file,"wget -r %s/update",error_report_ip);
                    sprintf(file,fooo1,error_report_ip);
                    system(file);
                    memset(file,0,sizeof(file));
                    //sprintf(file,"mv %s/update/* /usr/lib",error_report_ip);
                    sprintf(file,fooo2,error_report_ip);
                    system(file);

                    system(fooo3);
                    system(fooo4);
					gb2utf(v);
					system(fooo5);
                    close(sockfd);
                    exit(0);
                }
                else
                {
                  // printf("server return wrong!!!!\n");
				   sx=1;
                }
            }
        }
    }
    close(sockfd);
}

void empty()
{


}
/*********************
定时任务,任务是更新发送数据包到我们的服务器上
*********************/
int cache_count_flow(void)
{
    struct itimerval tv, otv;
    signal(SIGALRM,empty); ///ys 时间到了后，执行的函数是 report_error

    tv.it_value.tv_sec = 9;///ys 程序启动4秒后跑第一次
    tv.it_value.tv_usec = 0;

    tv.it_interval.tv_sec = 60*60*3; ///ys第一次,过后,以后每隔3个钟多久跑一次，该值在数据库里
    tv.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &tv, &otv) != 0) ///YS 系统调用，设置定时
        dbug("setitimer err %d\n", errno);

    return 0;
}

/*

int ReplaceStr(char *sSrc, char *sMatchStr, char *sReplaceStr)
{
    int  StringLen;
    char caNewString[1024*1024*2];

    char *FindPos = strstr(sSrc, sMatchStr);
    if( (!FindPos) || (!sMatchStr) )
        return -1;

    while( FindPos )
    {
        memset(caNewString, 0, sizeof(caNewString));
        StringLen = FindPos - sSrc;
        strncpy(caNewString, sSrc, StringLen);
        strcat(caNewString, sReplaceStr);
        strcat(caNewString, FindPos + strlen(sMatchStr));
        strcpy(sSrc, caNewString);

        FindPos = strstr(sSrc, sMatchStr);
    }

    return 0;
}


int out_put_file(char * filename,char *in_conten,long len)
{
    FILE *stream;
    stream = fopen(filename,"w+");
    fwrite(in_conten,len,1,stream);
    fclose(stream);
    return 0;
}

int replace_name(char *in_conten,long len,char *match_str,char *new_str)
{
    //dbug("%s\n",in_conten);
    //dbug("%s\n",strstr(in_conten,"pthread_cond_signal"));
    ReplaceStr(in_conten,match_str,new_str);
    return 0;
}







int process_file_new(char * file_name)
{
    char *in_file_content=NULL;
    long in_file_len=0;
    char new_filename[1024]={0};
    char new_filename2[1024]={0};

    char for_str[]={'%','s','_','n','e','w','.','c','\0'};
    snprintf(new_filename2,strlen(file_name)-1,"%s",file_name);
    sprintf(new_filename,for_str,new_filename2);
    dbug("%s\n",new_filename);
    get_file_content(file_name,&in_file_len,&in_file_content);
    format_filename(in_file_content,in_file_len);
    free(in_file_content);
    return 0;
}


int sum2(char *file,long len)
{
    int i;
    unsigned  char  temp[512]= {0};
    long sum=0;
    char temp2[512]= {0};
    char public_temp_buff[512];
    memset(public_temp_buff,0,521);
    MD5((unsigned char *)file,len,temp);
    for(i=0; i<32; i++)
    {
        sprintf(temp2,"%2.2x",temp[i]);
        strcat(public_temp_buff,temp2);
    }
    public_temp_buff[32]='\0';
    for(i=0;i<32;i++)
    {
        if(public_temp_buff[i]>90)
        {
                public_temp_buff[i]='1';
        }
    }
    char temp_sum[16];
    int j=0;
    for(i=0;i<32;i=i+2)
    {
        if((public_temp_buff[i]-0x30)+(public_temp_buff[i+1]-0x30)>9)
        temp_sum[j]=public_temp_buff[i];
        else
        temp_sum[j]=(public_temp_buff[i]-0x30)+(public_temp_buff[i+1]-0x30)+0x30;
        j++;
    }

    char temp_sum2[8];
    j=0;
    for(i=0;i<16;i=i+2)
    {
        if((temp_sum[i]-0x30)+(temp_sum[i+1]-0x30)>9)
        temp_sum2[j]=temp_sum[i];
        else
        temp_sum2[j]=(temp_sum[i]-0x30)+(temp_sum[i+1]-0x30)+0x30;
        j++;
    }
    sum=atol(temp_sum2);

    //dbug("44444444444444444444\n\n %ld ##  %s ## %ld\n",aa,public_temp_buff,sum);
return 0;
}


*/
