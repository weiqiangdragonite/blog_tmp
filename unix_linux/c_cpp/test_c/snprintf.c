#include <stdio.h>

int
main(void)
{
    char response[1024];
    char *msg = "hello %s, do you like %s?\n";
    snprintf(response, sizeof(response), msg, "dragonite");
    printf("%s\n", response);
    snprintf(response, sizeof(response), msg, "chicken");
    printf("%s\n", response);

    return 0;
}
