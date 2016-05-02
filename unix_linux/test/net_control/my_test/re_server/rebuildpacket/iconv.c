///YS 该文件的函数本来是做字符编码转换的，不过现在不用了，由PHP端来实现
#include <iconv.h>
#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/*
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
{
iconv_t cd;
int rc;
 char **pin = &inbuf;
char **pout = &outbuf;
cd = iconv_open(to_charset,from_charset);
if (cd==0) return -1;
memset(outbuf,0,outlen);
//dbug("pin   len   is   %s   %d\n",inbuf,inlen);
if (iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;
iconv_close(cd);
return 0;
}





int u2g(char *inbuf,int inlen,char *outbuf,int outlen)
{
return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);
}

int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);
}


int my_iconv(char *iconv_str)
{

char out_put[file_name_str_len];
char temp[file_name_str_len];
g2u(iconv_str,strlen(iconv_str),out_put,file_name_str_len);//gbk->utf8
strcpy(temp,out_put);
u2g(temp,strlen(temp),out_put,file_name_str_len);//utf8--->gbk
if(!strcmp(out_put,iconv_str))
{
    g2u(iconv_str,strlen(iconv_str),out_put,file_name_str_len);//gbk->utf8
    strcpy(iconv_str,out_put);
    return 1;
}
else
{
    return 0;
}


}
*/
