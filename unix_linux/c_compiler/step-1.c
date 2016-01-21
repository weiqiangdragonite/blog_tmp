/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/*
 * global variables
 */

int token = 0;		/* current token */
char *src = NULL;	/* pointer to source code string */
char *old_src = NULL;	/* pointer to source code string */
int poolsize = 0;	/* default size of text/data/stack */
int line = 0;		/* line number */


int *text = NULL;	/* text segment */
int *old_text = NULL;	/* for dump txt segment */
int *stack = NULL;	/* stack */
char *data = NULL;	/* data segment */


/* virtual machine registers*/
int *pc = NULL;		/* 程序计数器 */
int *sp = NULL;		/* 指针寄存器 */
int *bp = NULL;		/* 基址指针 */
int ax = 0;		/* 通用寄存器 */
int cycle = 0;


/* instructions */
enum { 	LEA, IMM, JMP, CALL, JZ, JNZ, ENT, ADJ, LEV, LI, LC, SI, SC, PUSH,
	OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR, ADD, SUB, MUL, DIV,
	MOD, OPEN, READ, CLOS, PRTF, MALC, MSET, MCMP, EXIT };


/*
 * function prototypes
 */
void next(void);
void program(void);
int eval(void);
void expression(int level);


int
main(int argc, char *argv[])
{
	int i, fd;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s source_file\n", argv[0]);
		return -1;
	}

	--argc;
	++argv;

	/* arbitrary size */
	poolsize = 256 * 1024;
	line = 1;

	if ((fd = open(*argv, O_RDONLY)) < 0) {
		fprintf(stderr, "could not open %s\n", *argv);
		return -1;
	}

	if ((src = old_src = malloc(poolsize)) == NULL) {
		fprintf(stderr, "could not malloc(%d) for source area\n", poolsize);
		return -1;
	}

	/* read the source file */
	if ((i = read(fd, src, poolsize - 1)) <= 0) {
		fprintf(stderr, "read() return %d\n", i);
		return -1;
	}
	/* add EOF */
	src[i] = '\0';
	close(fd);


	/* allocate memory for virtual machine */
	if ((text = old_text = malloc(poolsize)) == NULL) {
		fprintf(stderr, "Could not malloc(%d) for text area\n", poolsize);
		return -1;
	}
	if ((data = malloc(poolsize)) == NULL) {
		fprintf(stderr, "Could not malloc(%d) for data area\n", poolsize);
		return -1;
	}
	if ((stack = malloc(poolsize)) == NULL) {
		fprintf(stderr, "Could not malloc(%d) for stack area\n", poolsize);
		return -1;
	}

	memset(text, 0, poolsize);
	memset(data, 0, poolsize);
	memset(stack, 0, poolsize);

	/* init stack pointer */
	sp = bp = (int *) ((int) stack + poolsize);
	ax = 0;


	program();
	return eval();
}


/*
 * 词法分析，获取下一个标记，它将自动忽略空白字符。
 */
void
next(void)
{
	token = *src++;
	return;
}

/*
 * 语法分析的入口，分析整个 C 语言程序。
 */
void
program(void)
{
	next();	/* get next token */
	while (token != '\0') {
		printf("token is: %c\n", token);
		next();
	}
}

/*
 * 用于解析一个表达式
 */
void
expression(int level)
{
	/* do nothing */
}

/*
 * 虚拟机的入口，用于解释目标代码
 */
int
eval(void)
{
	/* do nothing */
	return 0;
}



















