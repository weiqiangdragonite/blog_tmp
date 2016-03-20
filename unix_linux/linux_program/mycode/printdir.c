/*
 * 扫描目录文件

struct dirent {
	ino_t		d_ino;		// inode number
	off_t		d_off;		// not an offset; see NOTES
	unsigned short	d_reclen;	// length of this record
	unsigned char	d_type;		// type of file; not supported
						by all filesystem types
	char		d_name[256];	// filename
};




 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>


void printdir(char *dir, int depth);

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <path>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	printdir(argv[1], 0);

	return 0;
}


/*
 * depth: number of space
 */
void
printdir(char *dir, int depth)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;

	if ((dp = opendir(dir)) == NULL) {
		perror("opendir() failed");
		return;
	}

	/* change directory to XXX, same as shell: cd XXX */
	chdir(dir);
	while ((entry = readdir(dp)) != NULL) {
		/* get file property */
		if (lstat(entry->d_name, &statbuf) == -1) {
			fprintf(stderr, "lstat() failed  - %s: %s\n",
				entry->d_name, strerror(errno));
			break;
		}

		if (S_ISDIR(statbuf.st_mode)) {
			/* find directory, ignore . and .. */
			if (strcmp(".", entry->d_name) == 0 ||
			strcmp("..", entry->d_name) == 0)
				continue;

			/* you can make a test: printf("%*shello\n", 4, " "); */
			/* this will print:    hello --> 4 space before hello */
			printf("%*s%s/\n", depth, "", entry->d_name);
			/* recurse at the new directory */
			printdir(entry->d_name, depth + 4);
		} else {
			printf("%*s%s\n", depth, "", entry->d_name);
		}
	}
	/* don't forget to go back to parent directory to continue */
	chdir("..");
	closedir(dp);
}


