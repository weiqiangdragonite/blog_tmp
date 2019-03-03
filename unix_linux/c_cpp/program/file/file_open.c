/*
 * File: file_open.c
 * -------------------
 * This program open file using Linux fuction.
 * By http://enjoylinux.cn/
 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        puts("You haven't input the open filename, please try again!\n");
        return 1;
    }

    // if the file doesn't exist, we use O_CREAT and 0755 to create file
    // otherwise, we just use O_RDWR
    int file;
    if ((file = open(argv[1], O_CREAT | O_RDWR, 0755)) < 0) {
        perror("Open file failure!\n");
        return 2;
    } else {
        printf("Open file %d success!\n", file);
    }

    close(file);
    return 0;
}
