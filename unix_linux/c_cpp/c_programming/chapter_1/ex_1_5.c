/*
 * Print Fahrenheit-Celsius table, for fahr = 300, 280, ..., 20, 0
 */

#include <stdio.h>

int
main(int argc, char *argv[])
{
	int fahr;

	printf("%10s %10s\n", "Fahrenheit", "Celsius");
	for (fahr = 300; fahr >= 0; fahr -= 20)
		printf("%10d %10.1f\n", fahr, (5.0 / 9.0) * (fahr - 32.0));

	return 0;
}
