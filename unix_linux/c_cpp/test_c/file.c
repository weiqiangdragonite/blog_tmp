#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    char *path = "/var/www/html/test";
    int file_fd = open(path, O_RDONLY);
    if (file_fd == -1) {
        printf("open error\n");
    }
    printf("open success\n");

    close(file_fd);

    return 0;
}
