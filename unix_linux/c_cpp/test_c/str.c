#include <stdio.h>

void send(char *str);
void hello();

int main(void)
{
    
    send("Hello, world\n");
    hello();

    return 0;
}

void send(char *str)
{
    for (int i = 0; str[i] != '\n'; ++i) {
        printf("%c", str[i]);
    }
    printf("\n");

    return;
}
