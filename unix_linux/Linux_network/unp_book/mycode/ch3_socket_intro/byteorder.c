/*
 * 其实还不确定
 */

#include <stdio.h>

int
main(void)
{
        short s = 0x0102;
        char *c = (char *) &s;;
        if (*c == 0x02)
                printf("little\n");
        else if (*c == 0x01)
                printf("big\n");
        return 0;
}

