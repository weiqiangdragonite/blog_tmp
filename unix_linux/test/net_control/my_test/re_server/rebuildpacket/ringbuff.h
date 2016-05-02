#ifndef RINGBUFF_H
#define RINGBUFF_H
#include "packet_info.h"
typedef struct {
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



#endif
