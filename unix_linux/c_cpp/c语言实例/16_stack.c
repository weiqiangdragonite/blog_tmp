/*
 * Stack
 * 
 * 最先存放到栈中的数据最后被拿出。这便是栈的特点：先进后出。
 * 栈的顺序存储是通过一个全局数组实现的，栈的大小就也就是数组的长度，
 * 可以自己定义。入栈时要确认栈是否已经满了，不然会有溢出。
 * 出栈时要确认栈是否已经空了，不然会有溢出。
 */



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


struct node {
	int data;
	struct node *next;
};

/*
 * 可以只用一个头指针就行，使用头插法进行插入
 * 如果使用尾插法，就需要定义一个尾指针，从尾指针处插入和取数据 (尾插法删除操作不方便)
 */
struct stack_list {
	struct node *head;
	struct node *tail;
};


struct stack_list *init_stack_list(void)
{
	struct stack_list *stack = 
		(struct stack_list *) malloc(sizeof(struct stack_list));
	if (stack == NULL) {
		perror("malloc() failed");
		return NULL;
	}

	stack->head = NULL;
	stack->tail = NULL;
	return stack;
}


int push_stack_list(struct stack_list *stack, int data)
{
	struct node *ptr = (struct node *) malloc(sizeof(struct node));
	if (ptr == NULL) {
		perror("malloc() failed");
		return -1;
	}
	ptr->data = data;
	ptr->next = NULL;

	// 尾插法
	if (stack->head == NULL && stack->tail == NULL) {
		stack->head = stack->tail = ptr;
	} else {
		stack->tail->next = ptr;
		stack->tail = ptr;
	}

	return 0;
}

int pop_stack_list(struct stack_list *stack, int *data)
{
	if (stack->tail == NULL) {
		fprintf(stderr, "stack is empty\n");
		return -1;
	}

	struct node *ptr = stack->tail;
	*data = ptr->data;

	ptr = stack->head;
	while (1) {
		if (ptr->next == stack->tail || ptr->next == NULL)
			break;
		ptr = ptr->next;
	}
	if (ptr->next == NULL) {
		stack->tail = stack->head = NULL;
	} else if (ptr->next == stack->tail) {
		/* be careful! */
		stack->tail = ptr;
		ptr = ptr->next;
		stack->tail->next = NULL;
	}
	free(ptr);

	return 0;
}

int delete_stack_list(struct stack_list *stack)
{
	if (stack->tail != NULL) {
		struct node *ptr = stack->head;
		while (ptr) {
			stack->head = ptr->next;
			free(ptr);
			ptr = stack->head;
		}
		stack->head = stack->tail = NULL;
	}

	free(stack);
	return 0;
}


/******************************************************************************/

// 下面是使用头插法的，可以对比一下

struct stack_list_2 {
	struct node *head;
};


struct stack_list_2 *init_stack_list_2()
{
	struct stack_list_2 *stack;

	stack = (struct stack_list_2 *) malloc(sizeof(struct stack_list_2));
	if (stack == NULL) {
		perror("malloc failed");
		return NULL;
	}
	stack->head = NULL;
	return stack;
}

int push_stack_list_2(struct stack_list_2 *stack, int data)
{
	struct node *ptr;
	ptr = (struct node *) malloc(sizeof(struct node));
	if (ptr == NULL) {
		perror("malloc() failed");
		return -1;
	}
	ptr->data = data;
	ptr->next = stack->head;
	stack->head = ptr;
	return 0;
}

int pop_stack_list_2(struct stack_list_2 *stack, int *data)
{
	if (stack->head == NULL) {
		fprintf(stderr, "stack is empty\n");
		return -1;
	}

	struct node *ptr = stack->head;
	*data = ptr->data;
	stack->head = ptr->next;
	free(ptr);
	return 0;
}

int delete_stack_list_2(struct stack_list_2 *stack)
{
	struct node *ptr;
	if (stack->head != NULL) {
		while (stack->head) {
			ptr = stack->head;
			stack->head = ptr->next;
			free(ptr);
		}
	}
	free(stack);
	return 0;
}


/******************************************************************************/



struct stack_array {
	int *data;
	int size;
	int sp;
};


struct stack_array *
init_stack_array(int size)
{
	struct stack_array *p;
	p = (struct stack_array *) malloc(sizeof(struct stack_array));
	if (p == NULL) {
		perror("malloc() failed");
		return NULL;
	}

	p->data = (int *) malloc(size * sizeof(int));
	if (p->data == NULL) {
		perror("malloc() failed");
		return NULL;
	}

	p->size = size;
	p->sp = 0;

	return p;
}

void delete_stack_array(struct stack_array *stack)
{
	if (stack != NULL) {
		if (stack->data != NULL)
			free(stack->data);
		free(stack);
	}
}

int push_stack_array(struct stack_array *stack, int data)
{
	if (stack->sp >= stack->size) {
		fprintf(stderr, "stack is full\n");
		return -1;
	}

	stack->data[stack->sp++] = data;

	return 0;
}

int pop_stack_array(struct stack_array *stack, int *data)
{
	if (stack->sp <= 0) {
		fprintf(stderr, "stack is empty\n");
		return -1;
	}

	*data = stack->data[--stack->sp];

	return 0;
}


/******************************************************************************/


void test1(void)
{
	int i, ret;
	struct stack_array *stack = init_stack_array(10);
	if (stack == NULL)
		return;

	for (i = 0; i < 10; ++i)
		push_stack_array(stack, i);
	push_stack_array(stack, 11);

	for (i = 0; i < 10; ++i) {
		pop_stack_array(stack, &ret);
		printf("stack[%d] = %d\n", i, ret);
	}
	pop_stack_array(stack, &ret);

	delete_stack_array(stack);
	stack = NULL;
}


void test2(void)
{
	int i, ret;
	struct stack_list *stack;

	stack = init_stack_list();
	if (stack == NULL)
		return;

	for (i = 0; i < 10; ++i)
		push_stack_list(stack, i);

	for (i = 0; i < 10; ++i) {
		pop_stack_list(stack, &ret);
		printf("stack[%d] = %d\n", i, ret);
	}
	pop_stack_list(stack, &ret);

	delete_stack_list(stack);
	stack = NULL;
}


void test3(void)
{
	int i, ret;
	struct stack_list_2 *stack;

	stack = init_stack_list_2();
	if (stack == NULL)
		return;

	for (i = 0; i < 10; ++i)
		push_stack_list_2(stack, i);

	for (i = 0; i < 10; ++i) {
		pop_stack_list_2(stack, &ret);
		printf("stack[%d] = %d\n", i, ret);
	}
	pop_stack_list_2(stack, &ret);

	delete_stack_list_2(stack);
	stack = NULL;
}

int main(int argc, char *argv[])
{
	test3();

	return 0;
}
