#include <stdio.h>

int
main(int argc, char *argv[])
{
	int i, j;
	int color, length;
	char white[] = "white";
	char black[] = "black";

	/* white is 1, black is -1 */
	length = 64;
	color = 1;
	for (i = 0; i < length / 8; ++i) {
		for (j = 0; j < 8; ++j) {
			if (color == 1)
				printf("%s ", white);
			else if (color == -1)
				printf("%s ", black);
			color = -color;
		}
		color = -color;
		printf("\n");
	}

	return 0;
}
