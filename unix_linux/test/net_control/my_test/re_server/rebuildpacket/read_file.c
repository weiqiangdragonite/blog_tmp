///YS 把临时文件从硬盘里读到内存

#include <stdio.h>
#include <stdlib.h>


/************************
把临时文件从硬盘里读到内存
参数1，是文件名
参数2，是文件的长度
参数3，是文件读入内存后所在的指针
*************************/
char *get_file_content (char *filename,long *len,char **buffer)
{
    FILE * pFile;
    long lSize;
    size_t result;
    ///YS 若要一个byte不漏地读入整个文件，只能采用二进制方式打开
    pFile = fopen (filename, "rb" );
    if (pFile==NULL)
    {
//        dbug ("the filename is %s   's len is NULL??\n",filename);
        return 0;
        //exit (1);
    }

    ///YS  获取文件大小
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    ///YS  分配内存存储整个文件

    *buffer = calloc (lSize,sizeof(char));
    if (*buffer == NULL)
    {
        fputs ("Memory error",stderr);
       // exit (2);
    }

    ///YS 将文件拷贝到buffer中
    result = fread (*buffer,1,lSize,pFile);
    if (result != lSize)
    {
        fputs ("Reading error",stderr);
     //   exit (3);
    }
    fclose (pFile);
    *len=lSize;///YS 返回长度
return 0;
}
