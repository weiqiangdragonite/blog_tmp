/*
 * File: fifo_write.c
 * -----------------
 * This program write data to fifo.
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(void)
{
    char buffer[80];
    // init buffer
    memset(buffer, 0, sizeof(buffer));

    char FIFO[] = "FIFO";

    // open fifo
    int fd;
    fd = open(FIFO, O_WRONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("cannot open fifo");
        return 1;
    }

    // write datas to fifo
    while (1) {
        // clear buffer
        memset(buffer, 0, sizeof(buffer));

        strcpy(buffer, "Hello world");
        if (write(fd, buffer, sizeof(buffer)) == -1) {
            //if (errno == EAGAIN) puts("no data yet!");
            perror("write data error");
        } else {
            printf("write to fifo data is: %s\n", buffer);
        }

        sleep(2);
    }

    close(fd);

    return 0;
}
