#include <stdio.h>

int main(void)
{
    int *ptr = NULL;
    int val = 1;
#ifdef __UNIX__
    printf("ptr = %ld\n", (long) var);
#else
    printf("no\n");
#endif

    return 0;
}
