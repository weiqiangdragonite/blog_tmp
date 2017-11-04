/*
 * 双向链表
 * 双向链表既可以沿着链表头部到链表尾部这样的方向进行操作，也可以沿
 * 着链表尾部到链表头部这样的方向进行操作。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


struct linked_list {
	int data;
	struct linked_list *next;
	struct linked_list *pre;
};



/*
 * 使用头指针不需要
 */
struct linked_list *init_linked_list(void)
{
	return NULL;
}


/*
 * 头插法
 */
int insert_linked_list(struct linked_list **list, int data)
{
	struct linked_list *ptr;
	ptr = (struct linked_list *) malloc (sizeof(struct linked_list));

	if (ptr == NULL) {
		perror("malloc() failed");
		return -1;
	}
	ptr->data = data;
	ptr->next = NULL;
	ptr->pre = NULL;

	if (*list != NULL) {
		ptr->next = *list;
		(*list)->pre = ptr;
	}
	*list = ptr;

	return 0;
}

/*
 * 删除指定元素
 * 不好
 */
void delete_element(struct linked_list **list, int data)
{
	struct linked_list *ptr, *old_ptr;
	ptr = old_ptr = *list;

	while (ptr) {
		if (ptr->data == data) {
			/* first node */
			if (ptr->pre == NULL) {
				/* only one node */
				if (ptr->next == NULL)
					*list = old_ptr = NULL;
				else {
					ptr->next->pre = NULL;
					*list = ptr->next;
				}
				free(ptr);
				ptr = *list;
				continue;
			/* last node */
			} else if (ptr->next == NULL) {
				old_ptr = ptr->pre;
				old_ptr->next = NULL;
			} else {
				old_ptr = ptr->pre;
				old_ptr->next = ptr->next;
				ptr->next->pre = old_ptr;
			}
			free(ptr);
			ptr = old_ptr;
		}
		if (ptr)
			ptr = ptr->next;
	}
}

void traverse_linked_list(struct linked_list *list)
{
	struct linked_list *ptr = list;
	while (ptr) {
		printf("%d ", ptr->data);
		ptr = ptr->next;
	}
	printf("\n");
	return;
}


void free_linked_list(struct linked_list *list)
{
	struct linked_list *ptr;
	ptr = list;
	while (ptr) {
		list = ptr->next;
		free(ptr);
		ptr = list;
	}

	return;
}



int main(int argc, char *argv[])
{
	int i;
	struct linked_list *list = NULL;

	for (i = 0; i < 10; ++i)
		insert_linked_list(&list, i);
	insert_linked_list(&list, 10);
	traverse_linked_list(list);

	delete_element(&list, 10);
	delete_element(&list, 5);
	delete_element(&list, 0);
	delete_element(&list, 100);
	traverse_linked_list(list);

	free_linked_list(list);
	list = NULL;

	return 0;
}

