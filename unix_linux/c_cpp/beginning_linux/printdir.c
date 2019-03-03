#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

void printdir(char *dir_name, int depth);

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s path\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	printdir(argv[1], 0);

	return 0;
}

void
printdir(char *dir_name, int depth)
{
	DIR *dp;
	struct dirent *dirent_ptr;
	struct stat stat_buf;

	/* Open directory stream */
	dp = opendir(dir_name);
	if (dp == NULL) {
		fprintf(stderr, "Cannot open %s: %s\n",
			dir_name, strerror(errno));
		return;
	}

	/* Change current directory */
	chdir(dir_name);
	while ((dirent_ptr = readdir(dp)) != NULL) {
		lstat(dirent_ptr->d_name, &stat_buf);
		if (S_ISDIR(stat_buf.st_mode)) {
			/* Found directory, ignore . and ..*/
			if (strcmp(".", dirent_ptr->d_name) == 0 ||
				strcmp("..", dirent_ptr->d_name) == 0)
				continue;
			printf("%*s%s/\n", depth, "", dirent_ptr->d_name);
			
			/* Recurse ar a new indent level */
			printdir(dirent_ptr->d_name, depth + 4);
		} else
			printf("%*s%s\n", depth, "", dirent_ptr->d_name);
	}
	/* Go back to parent directory */
	chdir("..");
	closedir(dp);
}
