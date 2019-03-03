#include <stdio.h>
#include <stdlib.h>

#define ENABLE	0

int
main(void)
{
	struct tcb {
		int counter;
		struct tcb *next;
	};

	struct tcb *test;
	test = malloc(sizeof(struct tcb));
	test->counter = 10;
	test->next = NULL;

	++(test->counter);
	printf("counter = %d\n", test->counter);
	free(test);

#if ENABLE
	printf("Enable\n");
#endif

#ifdef BUFSIZ
	long buf = BUFSIZ;
	printf("BUFSIZ = %ld\n", buf);
#endif
	return 0;
}
