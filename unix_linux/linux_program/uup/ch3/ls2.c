/*
 * ls2.c
 * purpose list contents of directory or directories
 * action if no args, use . list files in args
 * Notes: uses stat() and pwd.h and grp.h
 * BUG: try ls2 /tmp
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>


void do_ls(char *dirname);
void do_stat(char *fname);
void show_file_info(char *fname, struct stat *info);
void mode_to_letters(mode_t mode, char *str);
char *uid_to_name(uid_t uid);
char *gid_to_name(gid_t gid);


int
main(int argc, char *argv[])
{
	if (argc == 1)
		do_ls(".");
	else {
		while (--argc) {
			printf("%s:\n", *++argv);
			do_ls(*argv);
		}
	}

	return 0;
}

/*
 * list files in directory called dirname
 */
void
do_ls(char *dirname)
{
	DIR *dir_ptr;
	struct dirent *direntp;		/* each entry */

	if ((dir_ptr = opendir(dirname)) == NULL) {
		fprintf(stderr, "ls2: cannot open %s\n", dirname);
	} else {
		while ((direntp = readdir(dir_ptr)) != NULL)
			do_stat(direntp->d_name);
		closedir(dir_ptr);
	}
}


void
do_stat(char *fname)
{
	struct stat info;
	if (stat(fname, &info) == -1)
		fprintf(stderr, "stat(%s) failed: %s\n", fname, strerror(errno));
	else
		show_file_info(fname, &info);
}

void
show_file_info(char *fname, struct stat *info)
{
	char mode_str[11];
	mode_to_letters(info->st_mode, mode_str);

	printf("%s ", mode_str);

	printf("%4d ", (int) info->st_nlink);

	printf("%-8s ", uid_to_name(info->st_uid));
	printf("%-8s ", gid_to_name(info->st_gid));
	printf("%-8ld ", (long) info->st_size);
	printf("%.12s  ", 4 + ctime(&info->st_mtime));

	printf("%s\n", fname);
}



void
mode_to_letters(mode_t mode, char *str)
{
	strcpy(str, "----------");	/* default = no perms */

	if (S_ISDIR(mode))
		str[0] = 'd';		/* directory */
	if (S_ISCHR(mode))
		str[0] = 'c';		/* char devices */
	if (S_ISBLK(mode))
		str[0] = 'b';		/* block device */

	if (mode & S_IRUSR)		/* 3 bits for user */
		str[1] = 'r';
	if (mode & S_IWUSR)
		str[2] = 'w';
	if (mode & S_IXUSR)
		str[3] = 'x';

	if (mode & S_IRGRP)		/* 3 bits for group */
		str[4] = 'r';
	if (mode & S_IWGRP)
		str[5] = 'w';
	if (mode & S_IXGRP)
		str[6] = 'x';

	if (mode & S_IROTH)		/* 3 bits for other */
		str[7] = 'r';
	if (mode & S_IWOTH)
		str[8] = 'w';
	if (mode & S_IXOTH)
		str[9] = 'x';
}


char *
uid_to_name(uid_t uid)
{
	struct passwd *pw_ptr;
	static char numstr[10];		/* must use static for return */

	if ((pw_ptr = getpwuid(uid)) == NULL) {
		sprintf(numstr, "%d", uid);
		return numstr;
	}
	return pw_ptr->pw_name;
}

char *
gid_to_name(gid_t gid)
{
	struct group *grp_ptr;
	static char numstr[10];		/* must use static for return */

	if ((grp_ptr = getgrgid(gid)) == NULL) {
		sprintf(numstr, "%d", gid);
		return numstr;
	}
	return grp_ptr->gr_name;
}


