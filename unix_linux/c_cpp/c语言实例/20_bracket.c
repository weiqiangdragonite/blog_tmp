/*
 * 括号匹配。
 * 括号匹配使用了堆栈的原理。所谓的括号匹配，就是给了一连串括号，
 * 里面有各种类型的的括号，然后确定该串中的括号是否是一一 匹配的。
 *
 * 例如：({[]})这串括号中的括号就是匹配的。因为串中的括号都是成对出现。
 * (({)这串括号就不是匹配的，串中{没有与它配对的括号，而且与(匹配的
 * 括号数量也不正确
 *
 * 首先依次从串中读取括号，每次读取一个括号，如果读取的括号是左括号，
 * 比如(,{,[，那么就让括号入栈，如果读取的是右括号，比如),},]，那么
 * 就把栈顶的括号取出来，和它匹配，如果匹配，就继续进行判断串中的下
 * 一个括号，如果不匹配，那么就说明该串中的括号不匹配。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


typedef char DataType;

/******************************************************************************/

/* Stack 简单实现 */

struct node {
	DataType data;
	struct node *next;
};
struct stack_list {
	struct node *head;
};


struct stack_list *init_stack_list()
{
	struct stack_list *stack;

	stack = (struct stack_list *) malloc(sizeof(struct stack_list));
	if (stack == NULL) {
		perror("malloc failed");
		return NULL;
	}
	stack->head = NULL;
	return stack;
}

int push_stack_list(struct stack_list *stack, DataType data)
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

int pop_stack_list(struct stack_list *stack, DataType *data)
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

int free_stack_list(struct stack_list *stack)
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


int is_stack_empty(struct stack_list *stack)
{
	return stack->head == NULL ? 1 : 0;
}


/******************************************************************************/



// 支持匹配的括号 () , [] , {}, <>
// 要保证 BRACKETS 是偶数对
char BRACKETS[] = {
	'(', ')',
	'[', ']',
	'{', '}',
	'<', '>'
};

int is_bracket(char ch)
{
	int i;

	for (i = 0; i < sizeof(BRACKETS); i++) {
		if (ch == BRACKETS[i])
			return 1;
	}
	return 0;
}

int is_left_bracket(char ch)
{
	int i;
	for (i = 0; i < sizeof(BRACKETS); i += 2) {
		if (ch == BRACKETS[i])
			return 1;
	}
	return 0;
}

/*
 * 左右字符一定要匹配left和right
 * 而且要保证 BRACKETS 是偶数
 */
int is_pair(char left, char right)
{
	int i;
	for (i = 0; i < sizeof(BRACKETS); i += 2) {
		if (left == BRACKETS[i] && right == BRACKETS[i+1])
			return 1;
	}
	return 0;
}


/*
 * 判断是否匹配，匹配返回1，否则返回0，出错返回-1
 */
int is_bracket_match(char *str)
{
	struct stack_list *stack;
	int is_match = 0;
	char tmp;
	int i;

	stack = init_stack_list();
	if (stack == NULL)
		return -1;

	for (i = 0; str[i] != '\0'; ++i) {
		if (!is_bracket(str[i]))
			continue;

		if (is_left_bracket(str[i])) {
			if (push_stack_list(stack, str[i])) {
				// error - malloc失败
				free_stack_list(stack);
				return -1;
			}
		} else {
			if (pop_stack_list(stack, &tmp)) {
				// error - 栈为空
				is_match = 0;
				break;
			}
			if (is_pair(tmp, str[i])) {
				is_match = 1;
				continue;
			} else {
				// 不匹配
				is_match = 0;
				break;
			}
		}
	}

	/* 栈不为空的情况表明不匹配 */
	if (is_match == 1 && !is_stack_empty(stack))
		is_match = 0;
	free_stack_list(stack);

	return is_match;
}


void test()
{
	char *str_1 = "({[<>]})";
	char *str_2 = "(({)";
	char *str_3 = "[[123]456{dd}";
	int is_match = 0;

	is_match = is_bracket_match(str_1);
	if (is_match != -1)
		printf("%s is %s match\n", str_1, is_match ? "" : "not");

	is_match = is_bracket_match(str_2);
	if (is_match != -1)
		printf("%s is %s match\n", str_2, is_match ? "" : "not");

	is_match = is_bracket_match(str_3);
	if (is_match != -1)
		printf("%s is %s match\n", str_3, is_match ? "" : "not");
}

int main(int argc, char *argv[])
{
	test();

	return 0;
}

