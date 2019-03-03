#include <stdio.h>

int main(void)
{
    int val1 = 0177;
    printf("%o, %#o\n", val1, val1);

    int val2 = 0xff;
    printf("%x, %X, %#x, %#X\n", val2, val2, val2, val2);

    _Bool val3 = 0;
    printf("%i\n", val3);

    return 0;
}
