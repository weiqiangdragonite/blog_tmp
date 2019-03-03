#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *path = "/var/www/text/unkown/index.html";
    char *dir = strdup(path);
    char *base = strdup(path);
    printf("dir = %s, base = %s\n", dirname(dir), basename(base));

    printf("dir = %s\nbase = %s\n", dir, base);

    //
    //dir = dirname(dir);
    //base = basename(base);

    free(dir);
    free(base);

    return 0;
}
