#include <stdio.h>
#include "encrypt.h"

int main(void)
{
    char msg[80];
    while (fgets(msg, 80, stdin)) {
        encrypt(msg);
        printf("%s", msg);
    }

    return 0;
}
