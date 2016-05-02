/******************
队列的相关操作，不过现在已经换成了循环队列（ringbuff.c)，所以现在暂时没用
******************/

#include <stdio.h>
#include <stdlib.h>
#include "ringbuff.h"


extern void dbug(char *fmt,...);

/**************************
*ringbuff 初始化
*参数1 "cb"，是RB的指针
*参数2“size“，是RB的大小；
***************************/
void cbInit(CircularBuffer *cb, int size)
{
    cb->size  = size;
    cb->start = 0;
    cb->count = 0;
    cb->elems = (ElemType *)calloc(cb->size, sizeof(ElemType));

}


/************************
*判断ringbuff是否为满
************************/
int cbIsFull(CircularBuffer *cb)
{
    return cb->count == cb->size;
}


/************************
*判断ringbuff是否为空
*************************/
int cbIsEmpty(CircularBuffer *cb)
{
    return cb->count == 0;
}



/************************
*把参数elem写入到ringbuff
*参数ELEM是我们定义的结构体，mypacket_info的别名
*************************/
void cbWrite(CircularBuffer *cb, ElemType *elem)
{

    int end = (cb->start + cb->count) % cb->size;
    cb->elems[end] = *elem;
    if (cb->count == cb->size)
    {
        cb->start = (cb->start + 1) % cb->size;    /* full, overwrite */
        dbug("3");
    }
    else
        ++ cb->count;
}

/********************
*把参数elem从ringbuff中读出来
********************/
void cbRead(CircularBuffer *cb, ElemType *elem)
{
    *elem = cb->elems[cb->start];
    cb->start = (cb->start + 1) % cb->size;
    -- cb->count;
}

/********************
*free  ringbuff
********************/
void cbFree(CircularBuffer *cb)
{
    free(cb->elems); /* OK if null */
}

