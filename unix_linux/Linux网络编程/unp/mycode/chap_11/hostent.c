/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <netdb.h>
#include <arpa/inet.h>


int
main(int argc, char *argv[])
{
	char *ptr, **pptr;
	char str[INET_ADDRSTRLEN];
	struct hostent *hptr;

	while (--argc > 0) {
		ptr = *++argv;
		if ((hptr = gethostbyname(ptr)) == NULL) {
			fprintf(stderr, "gethostbyname() for %s failed: %s\n",
				ptr, hstrerror(h_errno));
			continue;
		}
		printf("official hostname: %s\n", hptr->h_name);

		for (pptr = hptr->h_aliases; *pptr != NULL; ++pptr)
			printf("\t aliases name: %s\n", *pptr);

		switch (hptr->h_addrtype) {
		case AF_INET:
			pptr = hptr->h_addr_list;
			for (; *pptr != NULL; ++pptr) {
				if (inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)) == NULL)
					fprintf(stderr, "inet_ntop() failed: %s\n",
						strerror(errno));
				else
					printf("\t address: %s\n", str);
			}
			break;
		default:
			fprintf(stderr, "unknown address type\n");
			break;
		}
	}

	return 0;
}





