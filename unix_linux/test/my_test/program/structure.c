

#include <stdio.h>

struct device {
	int id;
	int numbers;
	char type[20];
	char name[20];
};

// this whill init to all 0 (global and static will init to 0)
struct device dev_global;

void
print(struct device *d)
{
	char file[50];
	snprintf(file, sizeof(file), "%s", d->name);
	printf("file = %s\n", file);
}

int
main(void)
{
	// dev is not init to all 0
	struct device dev;
	printf("dev id = %d, numbers = %d\n", dev.id, dev.numbers);
	printf("dev global id = %d, number = %d\n", dev_global.id,
		dev_global.numbers);

	struct device *d;
	d = NULL;
	char file[50];
	snprintf(dev.name, sizeof(dev.name), "hello, world");
	printf("dev.name = %s\n", dev.name);
	d = &dev;
	snprintf(file, sizeof(file), "%s", d->name);
	printf("file = %s\n", file);

	print(d);

	return 0;
}
