/*
 * 循环链表
 * 
 * 对比一下单链接与循环链表的不同之处：单链表的尾结点哪里也没有指，
 * 因为它的next指针值为空。循环链表的尾结点指向了它的头结点。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


struct node {
	int data;
	struct node *next;
};

struct linked_list {
	struct node *head;
	struct node *tail;
};


/*
 * struct linked_list *list = init_linked_list();
 */
struct linked_list *init_linked_list(void)
{
	struct linked_list *list = NULL;
	list = (struct linked_list *) malloc(sizeof(struct linked_list));
	if (list == NULL) {
		perror("malloc() failed");
		return NULL;
	}
	list->head = NULL;
	list->tail = NULL;

	return list;
}


/*
 * 头插法
 */
int insert_linked_list(struct linked_list *list, int data)
{
	struct node *ptr;
	ptr = (struct node *) malloc (sizeof(struct node));
	if (ptr == NULL) {
		perror("malloc() failed");
		return -1;
	}
	ptr->data = data;
	ptr->next = NULL;

	if (list->head == NULL) {
		/* 如果这里采用尾插法，需要每次都把tail指向新结点 */
		list->tail = ptr;
	} else {
		ptr->next = list->head;
	}
	list->head = ptr;
	/* 最后一个节点指向第一个节点 */
	list->tail->next = list->head;

	return 0;
}

/*
 * 删除指定元素
 * 不好
 */
void delete_element(struct linked_list *list, int data)
{
	struct node *ptr, *old_ptr;
	ptr = old_ptr = list->head;

	while (ptr) {
		if (ptr->data == data) {
			/* delete first node */
			if (old_ptr == ptr) {
				/* only one node */
				if (ptr->next == ptr) {
					list->head = NULL;
					list->tail = NULL;
					ptr = NULL;
				} else {
					ptr = ptr->next;
					list->head = ptr;
					list->tail->next = list->head;
				}
				free(old_ptr);
				continue;

			/* delete last node */
			} else if (ptr->next == list->head) {
				list->tail = old_ptr;
				old_ptr->next = list->head;
				free(ptr);
				ptr = NULL;
				break;
			} else {
				old_ptr->next = ptr->next;
				free(ptr);
				ptr = old_ptr->next;
				continue;
			}
		}

		old_ptr = ptr;
		if (ptr == list->tail)
			break;
		else
			ptr = ptr->next;
	}
}

void traverse_linked_list(struct linked_list *list)
{
	struct node *ptr = list->head;
	while (ptr) {
		printf("%d ", ptr->data);
		if (ptr->next != list->head)
			ptr = ptr->next;
		else
			break;
	}
	printf("\n");
	return;
}


void free_linked_list(struct linked_list *list)
{
	struct node *ptr, *old_ptr;
	old_ptr = ptr = list->head;

	list->tail->next = NULL;
	while (ptr) {
		old_ptr = ptr->next;
		free(ptr);
		ptr = old_ptr;
	}
	free(list);

	return;
}



int main(int argc, char *argv[])
{
	int i;
	struct linked_list *list = NULL;

	list = init_linked_list();
	if (list == NULL) {
		fprintf(stderr, "init linked list failed\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < 10; ++i)
		insert_linked_list(list, i);
	insert_linked_list(list, 10);
	traverse_linked_list(list);

	delete_element(list, 10);
	delete_element(list, 5);
	delete_element(list, 0);
	delete_element(list, 100);
	traverse_linked_list(list);

	free_linked_list(list);
	list = NULL;

	return 0;
}

