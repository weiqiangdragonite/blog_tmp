#include <stdio.h>

#define SIZE 10

struct os_event {
	int id;
	int table[SIZE];
};

int
main(void)
{
	struct os_event table[10];
	printf("size = %d\n", sizeof(table));

	struct os_event *event_ptr;
	event_ptr = &table[2];
	printf("event_ptr->table size = %d\n", sizeof(event_ptr->table));

	return 0;
}
