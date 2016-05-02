/*﻿*************************************************************************************************************************
         1) 循环队列初始化
         2）入队操作
         3）出队操作
         4）判断是否为空队列
         5）判断队列是否已满
         6）遍历队列各元素
     注：为了避免队列空和满两个状态混淆，采用空闲一个位置的方式，即N个元素空间的循环队列最多只能存放N-1个有效元素。

 **************************************************************************************************************************/
#ifndef QUE_H
#define QUE_H
#include <stdbool.h>   //注意使用布尔类型时，需要引入此头文件



/* Queues.  */
#define QUEUE_SIZE        2048

struct queue
{
    unsigned char data[QUEUE_SIZE];
    int head, tail;
};

#define queue_wrap(x)        ((x) & (QUEUE_SIZE - 1))
#define queue_full(p)        (queue_wrap (p.head + 1) == p.tail)
#define queue_empty(p)          (p.head == p.tail)
#define queue_put(p, c)      (p.data[p.head] = c, p.head = queue_wrap (p.head + 1))
#define queue_get(p, c)      (c = p.data[p.tail], p.tail = queue_wrap (p.tail + 1))










typedef struct Queue
{
    struct packet_info * qBase;
    int front;
    int rear;
} QUEUE;
extern void enQueue(QUEUE *pq , struct packet_info packet);
extern void initQueue(QUEUE *pq);
extern bool isemptyQueue(QUEUE *pq);
extern bool is_fullQueue(QUEUE *pq);
extern u_char * getQueue(QUEUE *pq );
extern void traverseQueue( QUEUE *pq);
#endif
