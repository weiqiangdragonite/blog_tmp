/*
 * fileinfo.c - use stat() to obtain and print file properties
 *            - some membets are just numbers
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>


void show_stat_info(char *fname, struct stat *buf);


int
main(int argc, char *argv[])
{
	struct stat info;

	if (argc == 2) {
		if (stat(argv[1], &info) != -1) {
			show_stat_info(argv[1], &info);
		} else {
			perror("stat()");
			exit(EXIT_FAILURE);
		}
	} else {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}


/*
 * display some info from stat in a name - value format
 */
void
show_stat_info(char *fname, struct stat *buf)
{
	printf("mode: %o\n", buf->st_mode);
	printf("links: %d\n", buf->st_nlink);
	printf("user: %d\n", buf->st_uid);
	printf("group: %d\n", buf->st_gid);
	printf("size: %d\n", buf->st_size);
	printf("modtime: %d\n", buf->st_ctim);
	printf("name: %s\n", fname);
}

