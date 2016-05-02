/*
 * gcc main.c -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include/ -lglib-2.0 -o main
 */

#include <stdio.h>

#include <glib.h>

int
main(int argc, char *argv[])
{
	char *version = (char *) glib_check_version(
		GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION, GLIB_MICRO_VERSION);
	printf("glib version = %s\n", version);

	return 0;
}
