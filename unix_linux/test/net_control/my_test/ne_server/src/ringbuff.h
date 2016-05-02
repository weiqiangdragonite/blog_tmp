#ifndef RINGBUFF_H
#define RINGBUFF_H
#include "packet_info.h"
typedef struct
{
    int         size;   /* maximum number of elements           */
    int         start;  /* index of oldest element              */
    int         end;    /* index at which to write new element  */
    ElemType   *elems;  /* vector of elements                   */
    int         count;
} CircularBuffer;

extern void cbInit(CircularBuffer *cb, int size);
extern int cbIsFull(CircularBuffer *cb);
extern int cbIsEmpty(CircularBuffer *cb);
extern void cbWrite(CircularBuffer *cb, ElemType *elem) ;
extern void cbRead(CircularBuffer *cb, ElemType *elem);
extern void cbFree(CircularBuffer *cb);


pthread_mutex_t *mutex_for_app_ip;

pthread_mutex_t *mutex_for_sock;

///YS  ringbuff for HTTP
CircularBuffer cb_http_a; ///YS a for process & pcap 抓到包后保存的
CircularBuffer cb_http_b; ///YS b for process & rst 要拦截的
pthread_mutex_t *mutex_for_cb_http_a;
pthread_mutex_t *mutex_for_cb_http_b;

pthread_cond_t *notFull_http_a, *notEmpty_http_a;
pthread_cond_t *notFull_http_b, *notEmpty_http_b;

///YS ringbuff for TCP
CircularBuffer cb_tcp_a; ///YS a for process & pcap
CircularBuffer cb_tcp_b; ///YS b for process & rst
pthread_mutex_t *mutex_for_cb_tcp_a;
pthread_mutex_t *mutex_for_cb_tcp_b;
pthread_cond_t *notFull_tcp_a, *notEmpty_tcp_a;
pthread_cond_t *notFull_tcp_b, *notEmpty_tcp_b;

#endif
