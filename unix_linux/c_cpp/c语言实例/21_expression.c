/*
 * 表达式求值
 *
 * 这里说的表达式为包含加，减，乘除的四则运算表达式。例如：1+2*3-4/5
 * 就是一个四则运算表达式。这个表达式中，运算符在数字中间，所以我们
 * 叫它中缀表达式，这也是符合我们思维的一种表现形式，不过，计算机就
 * 不理解中缀表达式了，因为它没有办法像我们一样先进行乘除运算，
 * 然后再进行加减运算，所以我们需要把中缀表达式转换成计算机能理解的
 * 后缀表达式。
 *
 * “什么是后缀表达式”，所谓的后缀表达式就是运算符在数字后面。例如：
 * " 1+2*3-6/3 "这个中缀表达式可以为 " 123*+63/- " 这种后缀表达式.
 * 后缀表达式也叫逆波兰表达式。
 *
 * 表达式求值有两个大的步骤：
 *   中缀表达式转换为后缀表达式。
 *   对后缀表达式进行求值。
 *
 * 这两个大的步骤中还有一些小的步骤，接下来我们详细说说这些小步骤。
 * 在说之前，我首先说明一个概念：优先级。优先级代表着先后顺序，
 * 优先级在表达式运算过程中体现为：乘法和除法的优先级比加法和减法的优先级高。
 * 也就是我们通常说的先乘除后加减。我们在表达式求值过程中把中缀表达式转换
 * 为后缀表达式也与优先级有关，因为后缀表达式可以去掉运算符的优先级。
 * 没有优先级了，计算机就能理解后缀表达式并对其进行相关的运算。
 *
 * 中缀表达式转换为后缀表达式的步骤如下：
 *   1.从头到尾依次遍历中缀表达式，每次从表达式中读取一个字符；
 *   2.判断步骤1中读取的字符，如果是数字则保存到数组a中，如果是+*等运算符，
 *     请看下一个步骤；
 *   3.对存放运算符的栈进行出栈操作，把步骤的2中的运算符和刚才出栈的运算符
 *     进行优先级比较；
 *   4.如果步骤2中的运算符优先级高，那么把参与比较的这两个运算符都入栈。
 *     否则看下一步；
 *   5.如果步骤2中的运算符优先级低，那么让栈中的运算符继续出栈，并且把
 *     出栈的运算符存放数组a中；
 *   6.重复步骤4和步骤5,直到出栈运算符的优先级比步骤2中运算符的优先级高为止；
 *   7.重复步骤1到步骤6.直到遍历完中缀表达式为止；
 *   8.判断栈中是否还有运算符，如果有的话，就把所有运算符出栈，并且
 *     把出栈的运算符存放数组a中。
 *
 * 对后缀表达式求值的步骤如下：
 *   1.从头到尾依次遍历后缀表达式，每次从表达式中读取一个字符；
 *   2.判断步骤1中读取的字符，如果是数字则入栈，如果是+*等运算符，请看下一个步骤；
 *   3.对存放数字的栈进行两次出栈操作，依据步骤2中运算符的类型，对出栈的
 *     两个数字进行运算；
 *   4.对步骤3中的运算结果进行入栈操作，这样可以把运算结果保存到存放数字的栈中；
 *   5.重复步骤1到步骤4.直到遍历完后缀表达式为止；
 *   6.栈中最后一个元素就是该表达式的运算结果。
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>


/*
 * type 是存储的类型，0-运算符，1-数字
 */
typedef struct {
	char type;
	char op;
	int num;
} DataType;


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

int get_stack_list_item(struct stack_list *stack, DataType *data)
{
	if (stack->head == NULL) {
		fprintf(stderr, "stack is empty\n");
		return -1;
	}

	*data = stack->head->data;
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

char OP_CHARACTERS[] = {
	'*', '/',
	'+', '-'
};

int is_op(char ch)
{
	int i;
	for (i = 0; i < sizeof(OP_CHARACTERS); ++i) {
		if (ch == OP_CHARACTERS[i])
			return 1;
	}
	return 0;
}

/*
 * if op1 priority is higher than op2, return 1;
 * is equal, return 0;
 * is lower, return -1;
 * 因为我这里只支持 + - * / , 我在 OP_CHARACTERS 按照优先级定义了顺序，
 * 所以只需找到他们对应的位置作比较，越小的优先级越高，即 * 的优先级比 / 高，
 * + 的优先级比 - 高。
 */
int cmp_op(char op1, char op2)
{
	int pos_1, pos_2, i;
	for (i = 0; i < sizeof(OP_CHARACTERS); ++i) {
		if (op1 == OP_CHARACTERS[i])
			pos_1 = i;
		if (op2 == OP_CHARACTERS[i])
			pos_2 = i;
	}
	if (pos_1 < pos_2)
		return 1;
	else if (pos_1 == pos_2)
		return 0;
	else if (pos_1 > pos_2)
		return -1;
}

void reverse_str(char *str)
{
	int i, j, len, tmp;

	len = strlen(str);
	for (i = 0, j = len - 1; i < j; ++i, --j) {
		tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
	}

	return;
}


/*
 * 简单版，只支持 数字为0-9, 运算为 +, -, *, \ 的表达式
 * str: "1+2*3-6/3"
 * convert str to postfix expression
 * dest: "1 2 3 * + 6 3 / -"
 */
int to_postfix_expression(const char *str, char *dest, size_t len)
{
	struct stack_list *exp_stack, *op_stack, *res_stack;
	DataType data, tmp;
	int i, is_error, num, is_num;
	char op, ch;

	exp_stack = init_stack_list();
	if (exp_stack == NULL)
		return -1;
	op_stack = init_stack_list();
	if (op_stack == NULL)
		return -1;
	res_stack = init_stack_list();
	if (res_stack == NULL)
		return -1;

	/* 这里错误处理得不好 */
	is_error = 0;
	num = 0;
	is_num = 0;
	for (i = 0; str[i] != '\0'; ++i) {
		if (is_error) {
			fprintf(stderr, "process is error\n");
			break;
		}

		if (isdigit(str[i])) {
			num = num * 10 + (str[i] - '0');
			is_num = 1;

			//if (push_stack_list(exp_stack, str[i]) == -1) {
			//	/* error, free all */
			//	is_error = 1;
			//	break;
			//}
		} else if (is_op(str[i])) {
			/* 如果是符号的话，把之前的数字入栈 */
			if (is_num) {
				data.type = 1;
				data.num = num;
				if (push_stack_list(exp_stack, data) == -1) {
					/* error, free all */
					is_error = 1;
					break;
				}
				is_num = 0;
				num = 0;
			}


			data.type = 0;
			data.op = str[i];
			if (get_stack_list_item(op_stack, &tmp) == -1) {
				/* stack is empty, push it into op stack */
				if (push_stack_list(op_stack, data) == -1) {
					/* error, free all */
					is_error = 1;
					break;
				}
				continue;
			}
			/* 进行优先级比较 */
			if (cmp_op(data.op, tmp.op) > 0) { // careful
				/* str[i] > op, 优先级高，入栈 */
				if (push_stack_list(op_stack, data) == -1) {
					/* error, free all */
					is_error = 1;
					break;
				}
			} else {
				/* 如果优先级低，出栈，直到优先级比栈要高 */
				while (1) {
					if (pop_stack_list(op_stack, &tmp) == -1) {
						/* error, stack is empty, 这里栈不应该为空 */
						is_error = 1;
						break;
					}
					if (push_stack_list(exp_stack, tmp) == -1) {
						/* error */
						is_error = 1;
						break;
					}

					if (get_stack_list_item(op_stack, &tmp) == -1) {
						/* stack is empty */
						break;
					}
					if (cmp_op(data.op, tmp.op) > 0)
						break;
				}
				/* careful */
				if (push_stack_list(op_stack, data) == -1) {
					/* error, free all */
					is_error = 1;
					break;
				}
			}
		}
	}
	if (is_error) {
		free_stack_list(exp_stack);
		free_stack_list(op_stack);
		free_stack_list(res_stack);
		return -1;
	}
	/* 最后还有数字 */
	if (is_num) {
		data.type = 1;
		data.num = num;
		if (push_stack_list(exp_stack, data) == -1) {
			/* error, free all */
			free_stack_list(exp_stack);
			free_stack_list(op_stack);
			free_stack_list(res_stack);
			return -1;
		}
		is_num = 0;
		num = 0;
	}


	while (1) {
		/* op栈全部出栈 */
		if (pop_stack_list(op_stack, &data) == -1)
			/* stack is empty */
			break;
		if (push_stack_list(exp_stack, data) == -1) {
			/* error */
			free_stack_list(exp_stack);
			free_stack_list(op_stack);
			free_stack_list(res_stack);
			return -1;
		}
	}

	/* 要将栈逆序 */
	while (1) {
		if (pop_stack_list(exp_stack, &data) == -1)
			break;
		if (push_stack_list(res_stack, data) == -1) {
			/* error */
			free_stack_list(exp_stack);
			free_stack_list(op_stack);
			free_stack_list(res_stack);
			return -1;
		}
	}

	/* copy to dest */
	i = 0;
	memset(dest, 0, len);
	while (i < len - 1) {
		if (pop_stack_list(res_stack, &data) == -1)
			/* empty */
			break;

		if (data.type == 0)
			dest[i] = data.op;
		else if (data.type == 1)
			snprintf(dest+i, len-i, "%d", data.num);
		// add space 
		i = strlen(dest);
		if (i+1 >= len)
			break;
		dest[i++] = ' ';
	}
	dest[i] = '\0';

	free_stack_list(exp_stack);
	free_stack_list(res_stack);
	free_stack_list(op_stack);

	return 0;
}



/*
 * 后缀表达式求值
 * exp: "1 2 3 * + 6 3 / -"
 */
int eval_expression(char *exp)
{
	struct stack_list *num_stack;
	int i, is_error;
	char op, ch;

	num_stack = init_stack_list();
	if (num_stack == NULL)
		return -1;

	for (i = 0; exp[i] != '\0'; ++i) {
		if (isdigit(exp[i])) {
			if (push_stack_list(num_stack, str[i]) == -1) {
				/* error, free all */
			}
		} else if (is_op(str[i])) {
			/* careful 注意哪个是被加数，哪个是被减数 */
			if (pop_stack_list(num_stack, &num_2) == -1) {

			}
			if (pop_stack_list(num_stack, &num_1) == -1) {

			}
			answer = calculate(num_1, str[i], num_2);
			if (push_stack_list(num_stack, answer) == -1) {

			}
		}
	}

	pop_stack_list(num_stack, &answer)
}


void test_1()
{
	char *exp = "1 + 2 * 3 - 6 / 3";
	char postfix_exp[100];
	int ret;

	ret = to_postfix_expression(exp, postfix_exp, sizeof(postfix_exp));
	if (!ret)
		printf("postfix exp = %s\n", postfix_exp);
}


int main(int argc, char *argv[])
{
	test_1();

	return 0;
}

