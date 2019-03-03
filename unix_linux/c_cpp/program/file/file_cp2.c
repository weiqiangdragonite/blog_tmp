/*
 * File: file_cp2.c
 * ---------------
 * This program copy the file using c library.
 *
 * By Dragonite - weiqiangdragonite@gmail.com
 */

#include <stdio.h>

int main(int argc, char *argv[])
{
    // check the argument
    if (argc != 3) {
        printf("Usage: %s file_from file_to\n", argv[0]);
        return 1;
    }

    // open file
    FILE *file_in = fopen(argv[1], "r");
    if (file_in == NULL) {
        char msg[80];
        sprintf(msg, "Cannot openinng file '%s'", argv[1]);
        perror(msg);

        return 2;
    }

    // create the copy file, if the file exist, then open it
    FILE *file_to = fopen(argv[2], "w+");
    if (file_to == NULL) {
        char msg[80];
        sprintf(msg, "Cannot creating file '%s'", argv[2]);
        perror(msg);
        
        return 3;
    }

    // copy the file
    char buffer[256];
    while (1) {
        //
        size_t size = fread(buffer, sizeof(char), sizeof(buffer), file_in);
        printf("read size: %i\n", size);

        // end of file
        if (size == 0) break;

        size_t num;
        if (size == sizeof(buffer)) {
            num = fwrite(buffer, sizeof(char), sizeof(buffer), file_to);
        } else {
            num = fwrite(buffer, sizeof(char), size, file_to);
        }

        //size_t num = fwrite(buffer, 1, 1, file_to);
        printf("write num: %i\n", num);
        
        // we can use
        // while (!feof(file_in)) { ... }
    }

    puts("Copy file success!\n");
    
    // close file
    close(file_in);
    close(file_to);

    return 0;
}
