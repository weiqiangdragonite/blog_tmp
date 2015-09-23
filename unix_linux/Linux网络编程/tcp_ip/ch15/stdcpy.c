
#include <stdio.h>
#include <time.h>

#define BUF_SIZE	1024

int
main(int argc, char *argv[])
{
	FILE *fp1;
	FILE *fp2;
	char buf[BUF_SIZE];
	clock_t start, stop;
	double duration;

	fp1 = fopen("news.txt", "r");
	fp2 = fopen("copy2.txt", "w");


	start = clock();
	while (fgets(buf, BUF_SIZE, fp1) != NULL)
		fputs(buf, fp2);
	stop = clock();
	duration = (double) (stop - start) / CLOCKS_PER_SEC;

	printf("used time [3]: %f\n", duration);

	fclose(fp1);
	fclose(fp2);

	return 0;
}


