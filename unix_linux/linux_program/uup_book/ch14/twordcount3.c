/*
 * twordcount3.c - threaded word counter for two files, Version 3
 * one counter per file
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>


struct arg_set {
	char *fname;
	int count;
};

void *count_words(void *arg);

int
main(int argc, char *argv[])
{
	pthread_t t1, t2;
	struct arg_set args1, args2;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <file1> <file2>\n", argv[0]);
		exit(1);
	}

	args1.fname = argv[1];
	args1.count = 0;
	args2.fname = argv[2];
	args2.count = 0;

	pthread_create(&t1, NULL, count_words, (void *) &args1);
	pthread_create(&t2, NULL, count_words, (void *) &args2);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("%5d: %s\n", args1.count, args1.fname);
	printf("%5d: %s\n", args2.count, args2.fname);
	printf("%5d: total words\n", args1.count + args2.count);

	return 0;
}


void *
count_words(void *arg)
{
	struct arg_set *p = (struct arg_set *) arg;
	FILE *fp;
	int c, prevc = 0;

	if ((fp = fopen(p->fname, "r")) != NULL) {
		while ((c = getc(fp)) != EOF) {
			if (!isalnum(c) && isalnum(prevc))
				++p->count;
			prevc = c;
		}
		fclose(fp);
	} else
		perror("open() failed");

	return NULL;
}

