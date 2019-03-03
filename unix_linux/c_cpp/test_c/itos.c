#include <stdio.h>

int main(void)
{
    int data = 168;
    char num[10];
    int i = 0;
    while (1) {
        num[i++] = data % 10 + '0';
        data /= 10;
        if (data == 0) break;
    }

    for (i -= 1; i >= 0; --i) {
        printf("%c", num[i]);
    }
    puts("");

    return 0;
}
