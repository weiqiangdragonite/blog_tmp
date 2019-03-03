#include <stdio.h>

typedef unsigned char	u8;
typedef	unsigned int	u32;

//u8	array[10][10];


void create_mem(void *addr, u32 num_block, u32 block_len);

int
main(void)
{
	u8	array[10][20];
	printf("In the main():\n");
	printf("sizeof(array) = %ld\n", sizeof(array));
	printf("array start at: %p\n", &array);

	int i;
	for (i = 0; i < 10; ++i)
		printf("array[%d][0] = %p\n", i, &array[i][0]);

	printf("-----------------------\n");
	create_mem(array, 10, 20);


	return 0;
}

void
create_mem(void *addr, u32 num_block, u32 block_len)
{
	printf("In the create_mem():\n");
	printf("addr = %p\n", addr);

	void	**link_ptr;
	u8	*block_ptr;
	u32	loops;
	int	i;

	link_ptr = addr;
	block_ptr = (u8 *) addr;
	loops = num_block - 1;

	printf("link_ptr = %p\n", link_ptr);
	printf("block_ptr = %p\n", block_ptr);
	printf("loops = %d\n", loops);

	for (i = 0; i < loops; ++i) {
		block_ptr += block_len;
		*link_ptr = block_ptr;
		link_ptr = (void **) block_ptr;
	}
	link_ptr = NULL;

	link_ptr = addr;
	block_ptr = (u8 *) addr;
	for (i = 0; i < loops; ++i) {
		block_ptr += block_len;
		printf("link_ptr[%d][0] = %p\n", i, *link_ptr);
		link_ptr = (void **) block_ptr;
	}

	printf("----------------------\n");
	void *free_list = addr;
	for (i = 0; i < loops + 1; ++i) {
		block_ptr = free_list;
		free_list = *(void **) block_ptr;
		printf("block_ptr[%d] = %p\n", i, block_ptr);
	}
}
