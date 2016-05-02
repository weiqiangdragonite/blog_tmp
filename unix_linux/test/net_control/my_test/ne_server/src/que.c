/******************
队列的相关操作，不过现在已经换成了循环队列（ringbuff.c)，所以现在暂时没用
******************/

#include <stdio.h>
#include <stdlib.h>
#include "packet_info.h"
#include <stdbool.h>   //注意使用布尔类型时，需要引入此头文件
#include "que.h"
extern void dbug(char *fmt,...);


int num_of_que=0;
#define queue_len  1024
u_char * packet_temp;


bool is_fullQueue(QUEUE *pq)
{
    if((pq->rear +1)%queue_len == pq->front)
    {
        return true;
    }
    else
        return false;
}


void enQueue(QUEUE *pq , struct packet_info  packet)
{
    pq->qBase[pq->rear] = packet;
    pq->rear = (pq->rear + 1)%queue_len ;
    num_of_que++;
}


void initQueue(QUEUE *pq)
{
    pq->qBase = (struct packet_info *)malloc(sizeof(struct packet_info)*queue_len);
    if(pq->qBase == NULL)
    {
        dbug("init queue error！");
        exit(-1);
    }
    pq->front = pq->rear = 0;

}

bool isemptyQueue(QUEUE *pq)
{
    if(pq->front == pq->rear)
    {
        return true;
    }
    else
        return false;
}



/* void deQueue(QUEUE *pq , struct packet_info *packet)
{

    if(isemptyQueue(pq))
    {
        dbug(" queue is empty,delete stop!");
    }
    else
    {
        *packet = pq->qBase[pq->front];
        dbug(" delete packet %d ",packet->packet_id);
        pq->front = (pq->front + 1)%queue_len ;

    }

}*/


u_char * getQueue(QUEUE *pq )
{

    if(num_of_que <=0)
    {
        //dbug(" queue is empty,delete stop");
        return 0;
    }
    else
    {
        packet_temp = (pq->qBase[pq->front].raw_packet);
        pq->front = (pq->front + 1)%queue_len ;
        num_of_que--;
        return packet_temp;
    }

}


void traverseQueue( QUEUE *pq)
{
    if(isemptyQueue(pq))
    {
        dbug("queue is empty!\n");
        exit(0);
    }
    dbug("queue is :\n");
    dbug("front is %d,rear is%d :\n",pq->front,pq->rear);


    int tail = pq->front ;
    while(tail != pq->rear)
    {
        dbug(" %d ",pq->qBase[tail].packet_id);
        tail = (tail + 1)%queue_len;

    }
}
