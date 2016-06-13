/*
 * ./getopt_ex
 * -n ---> display "my name"
 * -g ---> display "my girlfriend"
 * -l ---> with parameter
 */

#include <stdio.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	int opt;
	char *pstr;

	pstr = NULL;

	while ((opt = getopt(argc, argv, ":ngl:")) != -1) {
		switch (opt) {
		case 'n':
			printf("my name\n");
			break;
		case 'g':
			printf("my girlfriend\n");
			break;
		case 'l':
			pstr = optarg;
			printf("%s\n", pstr);
			break;
		case ':':
			fprintf(stderr, "Missing argument (-%c)\n", optopt);
			break;
		case '?':
			fprintf(stderr, "Unrecognized option (-%c)\n", optopt);
			break;
		default:
			fprintf(stderr, "Unexpected case in switch()\n");
			break;
		}
	}

	printf("argc = %d, optind = %d\n", argc, optind);
	if (optind < argc) {
		printf("non-option ARGV-elements:\n");
		while (optind < argc) {
			printf("\"%s\" at argv[%d]\n", argv[optind], optind);
			++optind;
		}
	}

	return 0;
}

