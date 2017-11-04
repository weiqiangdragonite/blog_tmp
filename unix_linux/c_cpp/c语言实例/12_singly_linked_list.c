/*
 * 单链表
 * 单链表有两种实现方法，一种是线性存储，一种是链式存储。
 * 功能: 遍历链表，插入和删除元素，查找元素。
 *
 * 对比单链表的顺序储存和链式存储，可以看出来。
 *   顺序存储链表的优点：遍历链表方便，查找也方便。
 *   顺序存储链表的缺点：删除，插入元素比较繁琐。
 *   链式存储链表的优点：插入，删除元素比较方便。遍历和查找比较繁
 *   链式存储链表的缺点：遍历和查找元素比较繁。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/* 线性存储 */

struct array_list {
	int *data;
	int capacity;	/* nums of elements */
	int index;	/* current position */
};

/*
 * 初始化链表(申请数组空间)
 * 成功返回数组指针，否则返回NULL
 */
struct array_list *
init_array_list(int size)
{
	int *data = NULL;
	struct array_list *list = NULL;

	list = (struct array_list *) malloc(sizeof(struct array_list));
	if (list == NULL) {
		perror("malloc() failled");
		return NULL;
	}
	data = (int *) calloc(size, sizeof(int));
	if (data == NULL) {
		perror("calloc() failled");
		return NULL;
	}

	list->data = data;
	list->capacity = size;
	list->index = 0;

	return list;
}

/*
 * 尾插法插入元素
 * 成功返回元素下一位置(当前插入的元素数量)，否则返回-1
 */
int insert_array_list(struct array_list *list, int data)
{
	/* 如果空间不够我们可以实现动态增长的 */
	if (list->index >= list->capacity) {
		fprintf(stderr, "insert error\n");
		return -1;
	} else {
		list->data[list->index++] = data;
	}

	return list->index;
}

/*
 * 动态增长
 */
int insert_array_list_v2(struct array_list *list, int data)
{
	int i, *old_data, *new_data;

	/* 如果已用了一半空间，我们就增大空间 */
	if (list->index >= (list->capacity / 2)) {
		list->capacity = list->capacity * 2;
		new_data = (int *) calloc(list->capacity, sizeof(int));
		if (new_data == NULL) {
			fprintf(stderr, "expand array size failed\n");
			return -1;
		}
		/* 复制原数据 */
		for (i = 0; i < list->index; ++i)
			new_data[i] = list->data[i];
		old_data = list->data;
		list->data = new_data;
		free(old_data);
	}

	list->data[list->index++] = data;

	return list->index;
}


/*
 * 取出/删除指定位置元素
 * 成功返回0，否则返回-1
 */
int get_array_list(struct array_list *list, int pos, int *data)
{
	int i;
	if (pos >= list->index || pos < 0) {
		fprintf(stderr, "position not exist\n");
		return -1;
	}
	if (data != NULL)
		*data = list->data[pos];

	/* 将后面的元素往前移 */
	for (i = pos + 1; i < list->index; ++i)
		list->data[i - 1] = list->data[i];
	list->index--;

	return 0;
}

/*
 * 动态减少
 */
int get_array_list_v2(struct array_list *list, int pos, int *data)
{
	int i, *old_data, *new_data;
	if (pos >= list->index || pos < 0) {
		fprintf(stderr, "position not exist\n");
		return -1;
	}
	if (data != NULL)
		*data = list->data[pos];

	/* 将后面的元素往前移 */
	for (i = pos + 1; i < list->index; ++i)
		list->data[i - 1] = list->data[i];
	list->index--;

	/* 如果数量小于空间的1/4, 就把空间缩小一半 */
	if (list->index < (list->capacity / 4) && (list->capacity / 2) > 1) {
		list->capacity = list->capacity / 2;
		new_data = (int *) calloc(list->capacity, sizeof(int));
		if (new_data == NULL) {
			fprintf(stderr, "reduce array size failed\n");
			return -1;
		}
		/* 复制原数据 */
		for (i = 0; i < list->index; ++i)
			new_data[i] = list->data[i];
		old_data = list->data;
		list->data = new_data;
		free(old_data);
	}

	return 0;
}


/*
 * 查找元素
 * 若存在返回元素位置，否则返回-1
 */
int find_array_list(struct array_list *list, int data)
{
	int i;
	for (i = 0; i < list->index; ++i) {
		if (list->data[i] == data)
			return i;
	}

	return -1;
}


/*
 * 遍历元素
 * 其实可以把遍历函数想要的操作用函数指针传递进来
 */
void traverse_array_list(struct array_list *list)
{
	int i;
	for (i = 0; i < list->index; ++i)
		printf("%d ", list->data[i]);
	printf("\n");
}


/*
 * 删除链表
 */
void delete_array_list(struct array_list *list)
{
	if (list->data != NULL)
		free(list->data);
	if (list != NULL)
		free(list);
}


/******************************************************************************/


/* 链式存储 */

struct linked_list {
	int data;
	struct linked_list *next;
};


/*
 * 一般没必要初始化，直接 struct linked_list *list = NULL; 即可创建链表;
 * 除非是使用头结点，即struct linked_list list; 第一个结点用于指向链表，
 * 本身不存放数据，那就需要初始化。
 *
 * 使用头指针的方式如果是插入或删除的需要传递指针的指针来进行操作，
 * 使用头结点的方式只需要传递头结点的指针即可(方便链表操作)。
 */
struct linked_list *init_linked_list()
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

	if (*list == NULL) {
		*list = ptr;
	} else {
		ptr->next = *list;
		*list = ptr;
	}
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
			/* first */
			if (old_ptr == ptr)
				*list = old_ptr = ptr->next;
			else 
				old_ptr->next = ptr->next;

			free(ptr);
			ptr = old_ptr;
		} else {
			old_ptr = ptr;
			ptr = ptr->next;
		}
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
	struct linked_list *ptr = list;
	while (ptr) {
		list = ptr->next;
		free(ptr);
		ptr = list;
	}
	return;
}




/******************************************************************************/


void test3(void)
{
	struct linked_list *list = NULL;
	int i;

	for (i = 0; i < 10; ++i)
		insert_linked_list(&list, i);
	traverse_linked_list(list);

	insert_linked_list(&list, 100);
	delete_element(&list, 100);
	delete_element(&list, 5);
	delete_element(&list, 0);

	traverse_linked_list(list);
	free_linked_list(list);
	list = NULL;

	return;
}


void test2(void)
{
	int i, ret;
	struct array_list *list = init_array_list(10);
	if (list == NULL) {
		fprintf(stderr, "cannot init array list\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < 20; ++i) {
		ret = insert_array_list_v2(list, i);
		if (ret == -1)
			fprintf(stderr, "cannot insert array list\n");
	}
	traverse_array_list(list);

	ret = find_array_list(list, 25);
	printf("data 25 at position %d\n", ret);
	get_array_list_v2(list, ret, NULL);

	for (i = 15; i >= 0; --i) {
		ret = find_array_list(list, i);
		printf("data %d at position %d\n", i, ret);
		get_array_list_v2(list, ret, NULL);
	}

	traverse_array_list(list);
	printf("capacity = %d\n", list->capacity);
	delete_array_list(list);
	list = NULL;

}

void test1(void)
{
	int i, ret;
	struct array_list *list = init_array_list(10);
	if (list == NULL) {
		fprintf(stderr, "cannot init array list\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < 11; ++i) {
		ret = insert_array_list(list, i);
		if (ret == -1)
			fprintf(stderr, "cannot insert array list\n");
	}

	ret = find_array_list(list, 15);
	printf("data 15 at position %d\n", ret);
	get_array_list(list, ret, NULL);

	ret = find_array_list(list, 5);
	printf("data 5 at position %d\n", ret);
	get_array_list(list, ret, NULL);

	traverse_array_list(list);
	delete_array_list(list);
	list = NULL;
}

int main(int argc, char *argv[])
{
	test1();
	test2();
	test3();

	return 0;
}
