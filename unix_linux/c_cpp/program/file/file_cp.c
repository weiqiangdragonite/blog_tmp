/*
 * File: file_cp.c
 * ---------------
 * This program copy the file.
 *
 * By Dragonite - weiqiangdragonite@gmail.com
 */

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // check the argument
    if (argc != 3) {
        printf("Usage: %s file_from file_to\n", argv[0]);
        return 1;
    }

    // open file
    int fd_from = open(argv[1], O_RDONLY);
    if (fd_from < 0) {
        char msg[80];
        sprintf(msg, "Cannot openinng file '%s'", argv[1]);
        perror(msg);

        return 2;
    }

    // create the copy file, if the file exist, then open it
    int fd_to = open(argv[2], O_CREAT | O_WRONLY, 0755);
    if (fd_to < 0) {
        char msg[80];
        sprintf(msg, "Cannot creating file '%s'", argv[2]);
        perror(msg);
        
        return 3;
    }

    // copy the file
    char buffer[1024];
    while (1) {
        int size = read(fd_from, buffer, sizeof(buffer));
        printf("read size: %i\n", size);
        if (size == 0) break;
        int si = write(fd_to, buffer, size);
        printf("write size: %i\n", si);
    }

    puts("Copy file success!\n");
    
    // close file
    close(fd_from);
    close(fd_to);

    return 0;
}
