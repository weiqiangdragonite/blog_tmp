/*
 * File: fifo_read.c
 * -----------------
 * This program read data from fifo.
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

    // create fifo
    int fd;
    // delete FIFO
    unlink(FIFO);

    if (mkfifo(FIFO, 0755) < 0) {
        perror("cannot create fifo");
        return 1;
    }

    // open fifo
    fd = open(FIFO, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("cannot open fifo");
        return 2;
    }

    while (1) {
        // clear buffer
        memset(buffer, 0, sizeof(buffer));

        if (read(fd, buffer, sizeof(buffer)) == -1) {
            //if (errno == EAGAIN) puts("no data yet!");
            perror("no data yet!");
        } else {
            printf("read from fifo data is: %s\n", buffer);
        }

        sleep(1);
    }

    //
    pause();
    unlink(FIFO);
    close(fd);

    return 0;
}
