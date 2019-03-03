#include <stdio.h>
#include "point.h"

typedef unsigned char	u8;
typedef unsigned int	u32;

void mem_create(void *addr, u32 num_blocks, u32 block_len);

int main(void)
{
    list = NULL;
    table[0].x = 100;
    table[0].y = 2;
    table[0].next = NULL;
    list = table;

    printf("table[0].x = %d\n", list->x);

	printf("sizeof(long) = %ld\n", sizeof(long));

	long array[3][4] = { {1, 3, 5, 7}, {2, 4, 6, 8},
			     {5, 6, 7, 8} };
	long *p = &array[0][0];
	long **point = &p;

	printf("array address: %p\n", &array);
	printf("p point to: %p\n", p);
	printf("p address: %p\n", &p);

	printf("**point address: %p\n", &point);
	printf("**point point to: %p\n", point);
	printf("*point = %p\n", *point);
	printf("point[0][0] = %ld\n", **point);
	printf("point[0][1] = %ld\n", **point + 1);

	mem_create(array, 3, 4);

    return 0;
}

void mem_create(void *addr, u32 num_blocks, u32 block_len)
{
	printf("------------------------------\n");

	printf("addr point to: %p\n", addr);
	printf("addr address: %p\n", &addr);

	void	*link_ptr;
	u8	*block_ptr;
	u32	loops = num_blocks - 1;

	link_ptr = addr;
	block_ptr = (u8 *) addr;

//	printf("**link_ptr = %p\n", **link_ptr);
//	printf("*link_ptr = %ld\n", (long) *link_ptr);
	printf("link_ptr = %p\n", link_ptr);

	printf("block_ptr = %p\n", block_ptr);
	printf("*block_ptr = %u\n", *block_ptr);
	printf("*block_ptr[1] = %i\n", *(block_ptr + 8));
	
	printf("---------------------------------\n");

	int i;
	for (i = 0; i < loops; ++i) {
		block_ptr += block_len;
		printf("block_ptr = %p\n", block_ptr);

		//*link_ptr = (void *) block_ptr;
		//printf("link_ptr = %ld\n", *(long *) link_ptr);

		link_ptr = (void *) block_ptr;
		printf("link_ptr = %p\n", link_ptr);
	}
}
