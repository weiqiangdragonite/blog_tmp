/*
 * Circular buffer http://en.wikipedia.org/wiki/Circular_buffer
 * Ring buffer
 */

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "packet_info.h"

struct circular_buffer {
	unsigned int	size;
	unsigned int	start;
	unsigned int	end;
	unsigned int	count;
	ElemType	*elems;
};

typedef struct circular_buffer CircularBuffer;





/* circular buffer for http and tcp data after catch by pcap */
CircularBuffer cb_http_catch;
CircularBuffer cb_tcp_catch;
CircularBuffer cb_http_intercept;
CircularBuffer cb_tcp_intercept;

CircularBuffer cb_tcp_audit;


/* mutext for http and tcp circular buffer  */
pthread_mutex_t *mutex_cb_http_catch;
pthread_mutex_t *mutex_cb_http_intercept;
pthread_mutex_t *mutex_cb_tcp_catch;
pthread_mutex_t *mutex_cb_tcp_intercept;

pthread_mutex_t *mutex_cb_tcp_audit;

/* condition for circular buffer */
pthread_cond_t *not_full_http_catch, *not_empty_http_catch;
pthread_cond_t *not_full_http_intercept, *not_empty_http_intercept;
pthread_cond_t *not_full_tcp_catch, *not_empty_tcp_catch;
pthread_cond_t *not_full_tcp_intercept, *not_empty_tcp_intercept;


pthread_cond_t *not_full_tcp_audit, *not_empty_tcp_audit;


/* function prototypes */
void cb_init(CircularBuffer *cb, unsigned int size);
int cb_isfull(CircularBuffer *cb);
int cb_isempty(CircularBuffer *cb);
void cb_write(CircularBuffer *cb, ElemType *elem);
void cb_read(CircularBuffer *cb, ElemType *elem);
void cb_free(CircularBuffer *cb);


#endif	/* CIRCULAR_BUFFER_H */
