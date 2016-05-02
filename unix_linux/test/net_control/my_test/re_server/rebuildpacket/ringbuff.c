///YS 环型缓冲数据结构的代码在这
#include <stdio.h>
#include <stdlib.h>
#include "ringbuff.h"
/* Opaque buffer element type.  This would be defined by the application. */
//typedef struct { ; } ElemType;

/* Circular buffer object */



/*
*ringbuff 初始化，参数 cb ...，size  大小；
*/
void cbInit(CircularBuffer *cb, int size)
{
    cb->size  = size;
    cb->start = 0;
    cb->count = 0;
    cb->elems = (ElemType *)calloc(cb->size, sizeof(ElemType));

}


/*
*判断ringbuff是否为满
*/
int cbIsFull(CircularBuffer *cb)
{
    return cb->count == cb->size;
}


/*
*判断ringbuff是否为空
*/
int cbIsEmpty(CircularBuffer *cb)
{
    return cb->count == 0;
}




/*
*把参数elem写入到ringbuff
*/
void cbWrite(CircularBuffer *cb, ElemType *elem)
{

    int end = (cb->start + cb->count) % cb->size;
    cb->elems[end] = *elem;
    if (cb->count == cb->size)
    {
        cb->start = (cb->start + 1) % cb->size;    /* full, overwrite */
//        dbug("3");
    }
    else
        ++ cb->count;
}

/*
*把参数elem从ringbuff中读出来
*/
void cbRead(CircularBuffer *cb, ElemType *elem)
{
    *elem = cb->elems[cb->start];
    cb->start = (cb->start + 1) % cb->size;
    -- cb->count;
}

void cbFree(CircularBuffer *cb)
{
    free(cb->elems); /* OK if null */
}

