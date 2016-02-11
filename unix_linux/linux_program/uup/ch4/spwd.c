/*
 * spwd.c: a simplified version of pwd
 *
 * starts in current directory and recursively
 * climbs up to root of filesystem, prints top part
 * then printd current part
 *
 * uses readdir() to get info about each thing
 *
 * BUG: prints an empty string if run from '/'
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>


void print_path_to(ino_t this_inode);
char *inode_to_name(ino_t inode);
ino_t get_inode(char *fname);


int
main(int argc, char *argv[])
{
	print_path_to(get_inode("."));		/* print path to here */
	putchar('\n');
	return 0;
}


void
print_path_to(ino_t this_inode)
{
	ino_t my_inode;
	char name[256];


	if (get_inode("..") != this_inode) {
		chdir("..");			/* go to parent directory */
		strncpy(name, inode_to_name(this_inode), sizeof(name) - 1);
		//printf("indoe = %d, name = %s\n", this_inode, name);

		my_inode = get_inode(".");
		print_path_to(my_inode);	/* recursively */
		printf("/%s", name);
	} else {
		//strncpy(name, inode_to_name(this_inode), sizeof(name) - 1);
		//printf("indoe = %d, name = %s\n", this_inode, name);
		return;
	}
}

/*
 * looks through current directory for a file with this inode
 * number 
 */
char *
inode_to_name(ino_t inode)
{
	DIR *dir_ptr;
	struct dirent *dp;

	dir_ptr = opendir(".");
	if (dir_ptr == NULL) {
		perror("opendir() failed");
		exit(EXIT_FAILURE);
	}

	while ((dp = readdir(dir_ptr)) != NULL) {
		if (dp->d_ino == inode) {
			closedir(dir_ptr);
			return dp->d_name;
		}
	}
	fprintf(stderr, "error looking for inode number: %d\n", inode);
	exit(EXIT_FAILURE);
	//return (char *) NULL;
}



/*
 * returns indoe number of the file
 */
ino_t
get_inode(char *fname)
{
	struct stat info;
	if (stat(fname, &info) == -1) {
		fprintf(stderr, "Cannot stat(%s): %s\n", fname, strerror(errno));
		exit(EXIT_FAILURE);
	}
	return info.st_ino;
}
