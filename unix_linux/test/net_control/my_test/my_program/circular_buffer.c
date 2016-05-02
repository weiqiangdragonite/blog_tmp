/*
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "circular_buffer.h"


/*
 *
 */
void
cb_init(CircularBuffer *cb, unsigned int size)
{
	cb->size = size;
	cb->start = 0;
	cb->end = 0;
	cb->count = 0;
	/* calloc is same as malloc, except that malloc doesn't initialize the
	memory and calloc does! */
	cb->elems = (ElemType *) calloc(cb->size, sizeof(ElemType));
}

/*
 *
 */
int
cb_isfull(CircularBuffer *cb)
{
	return cb->count == cb->size;
}

/*
 *
 */
int
cb_isempty(CircularBuffer *cb)
{
	return cb->count == 0;
}

/*
 *
 */
void
cb_write(CircularBuffer *cb, ElemType *elem)
{
	int end;

	end = (cb->start + cb->count) % cb->size;
	cb->elems[end] = *elem;
	if (cb->count == cb->size)
		/* full, overwrite ??? I don't think so */
		cb->start = (cb->start + 1) % cb->size;
	else
		++(cb->count);
}

/*
 *
 */
void
cb_read(CircularBuffer *cb, ElemType *elem)
{
	*elem = cb->elems[cb->start];
	cb->start = (cb->start + 1) % cb->size;
	--(cb->count);
}

/*
 *
 */
void
cb_free(CircularBuffer *cb)
{
	/* OK if null */
	free(cb->elems);
}






