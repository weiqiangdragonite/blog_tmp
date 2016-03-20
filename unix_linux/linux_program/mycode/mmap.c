/*
 * 利用mmap和数组方式的存取操作来修改一个结构化数据文件
 *
 * 要拿到linux的文件夹下试，不要直接在共享文件夹内试
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#define NRECORDS	100

struct node {
	int i;
	char str[32];
};


int
main(int argc, char *argv[])
{
	struct node record, *mapped;
	int i, fd;
	FILE *fp;

	memset(&record, 0, sizeof(record));
	printf("size = %d\n", (int) sizeof(record));

	fp = fopen("records.dat", "w+");
	for (i = 0; i < NRECORDS; ++i) {
		record.i = i;
		snprintf(record.str, sizeof(record.str), "RECORD: %d\n", i);
		fwrite(&record, sizeof(record), 1, fp);
	}
	fclose(fp);


	/* 把第43条记录中的数值由43改为143 */
	fp = fopen("records.dat", "r+");
	fseek(fp, 43 * sizeof(record), SEEK_SET);
	fread(&record, sizeof(record), 1, fp);
	printf("record[43]: i = %d, str = %s\n", record.i, record.str);

	record.i = 143;
	snprintf(record.str, sizeof(record.str), "RECORD: %d\n", record.i);

	fseek(fp, 43 * sizeof(record), SEEK_SET);
	fwrite(&record, sizeof(record), 1, fp);
	fclose(fp);


	/* 将记录映射到内存中，将第43条记录（即143）改为234 */
	fd = open("records.dat", O_RDWR);
	if (fd == -1) {
		perror("open() failed");
		exit(EXIT_FAILURE);
	}
	mapped = (struct node *) mmap(NULL, NRECORDS * sizeof(record),
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mapped == MAP_FAILED) {
		perror("mmap() failed");

		switch (errno) {
		case EACCES:
			fprintf(stderr, "EACCES\n");
			break;
		case EBADF:
			fprintf(stderr, "EBADF\n");
			break;
		case EINVAL:
			fprintf(stderr, "EINVAL\n");
			break;
		case ENODEV:
			fprintf(stderr, "ENODEV\n");
			break;
		default:
			fprintf(stderr, "others\n");
			break;
		}

		exit(EXIT_FAILURE);
	}

	mapped[43].i = 243;
	snprintf(mapped[43].str, sizeof(mapped[43].str), "RECORD: %d\n", mapped[43].i);

	/* write to file */
	msync((void *) mapped, NRECORDS * sizeof(record), MS_ASYNC);
	/* free */
	munmap((void *) mapped, NRECORDS * sizeof(record));
	close(fd);

	return 0;
}


