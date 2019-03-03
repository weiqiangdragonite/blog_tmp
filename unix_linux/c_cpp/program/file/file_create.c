/*
 * File: file_create.c
 * -------------------
 * This program create file using Linux fuction.
 * By http://enjoylinux.cn/
 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* function prototypes */
void create_file(char *filename);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        perror("You haven't inpuit the filename, please try again!\n");
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        create_file(argv[i]);
    }

    return 0;
}

void create_file(char *filename)
{
    if (creat(filename, 0755) < 0) {
        printf("Create file %s failure!\n", filename);
        return;
    } else {
        printf("Create file %s success!\n", filename);
    }
}
