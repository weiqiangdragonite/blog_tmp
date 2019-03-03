#include <stdio.h>

void test(void);

int main(void)
{
    unsigned int addr[10];
    addr[0] = (unsigned int) test;

    printf("the addr[0] address = %#x\n", (unsigned int) (&addr));
    printf("the value of addr[0] = %#x\n", (unsigned int) addr[0]);
    printf("the test address = %#x\n", (unsigned int) (*addr));

    return 0;
}

void test(void)
{
    return;
}
