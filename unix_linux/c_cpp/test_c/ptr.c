#include <stdio.h>

unsigned int stack[100];

int main(void)
{
    char ch1 = 'A';
    char ch2 = 'B';

    const char *ptr1 = &ch1;
    // *ptr = 'C'; ->  compile error
    ptr1 = &ch2; // ok, we cannot change the object which we point to,
                // but we can change to point to another object
    printf("ch2 = %c\n", *ptr1);

    char * const ptr2 = &ch1;
    //ptr2 = &ch1; -> compile error
    *ptr2 = 'C'; // ok, we cannot change to point to another object,
                 // but we can change the object value
    printf("ch1 = %c\n", *ptr2);

    const char * const ptr3 = &ch1;
    //ptr3 = &ch2;  -> compile error, cannot change to point to another object
    //*ptr3 = 'A';  -> compile error, cannot change the object value
    printf("ch1 = %c\n", *ptr3);

    //
    unsigned long addr = (unsigned long) stack;
    printf("stack address = 0x%08x\n", &stack);
    printf("stack[1] = 0x%08x\n", &stack[1]);
    printf("addr = 0x%08x\n", addr);
    return 0;
}
