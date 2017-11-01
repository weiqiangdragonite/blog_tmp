/*
 * 单链表
 * 单链表有两种实现方法，一种是线性存储，一种是链式存储。
 * 功能: 遍历链表，插入和删除元素，查找元素。
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



/* 链式存储 */







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
	/*********/

	return 0;
}
