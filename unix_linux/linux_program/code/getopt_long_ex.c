/*
 * ./getopt_long_ex -h --help -f file1 --file file2 -n --number -a xxx xxx
 */


#include <stdio.h>
#include <getopt.h>


int
main(int argc, char *argv[])
{
	int opt;
	int option_index = 0;
	int number = 1;
	char *optstring = ":hf:na";
	char *pstr = NULL;

	// 数据结构的最后一个元素，要求所有域的内容均为0
	// The last element of the array has to be filled with zeros
	const struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"file", required_argument, NULL, 'f'},
		{"number", no_argument, &number, 9},
		{"arg", optional_argument, NULL, 'a'},
		{0, 0, 0, 0}
	};

	while (1) {
		opt = getopt_long(argc, argv, optstring, long_options,
			&option_index);
		if (opt == -1)
			break;

		// getopt_long() return the option  character when  a short
		// option is recognized. For a long option, they return val if
		// flag is NULL, and 0 otherwise.  Error and -1 returns are the
		// same as for  getopt() 
		switch (opt) {
		// 如果flag指针不为NULL，getopt_long()会使得它所指向的变量中
		// 填入val字段中的数值，并且getopt_long()返回0。如果flag不是
		// NULL，但未发现长选项，那么它所指向的变量的数值不变。
		case 0:
			printf("long option, number = %d\n", number);
			break;
		case 'h':
			printf("help\n");
			break;
		case 'f':
			pstr = optarg;
			printf("file name = %s\n", pstr);
			break;
		case 'n':
			printf("short option, number = %d\n", number);
			break;
		case 'a':
			printf("optional argument\n");
			//printf("option_index = %d\n", option_index);
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

		// 如果longindex非空，它指向的变量将记录当前找到参数符合
		// longopts里的第几个元素的描述，即是longopts的下标值。
		printf("option_index = %d\n", option_index);

		printf("\n");
	}

	return 0;
}