/*
 * Print Celsius-Fahrenheit table, for celsius = 0, 20, ..., 300
 */

#include <stdio.h>

int
main(int argc, char *argv[])
{
	float fahr, celsius;
	int lower, upper, step;

	lower = 0;		/* lower limit of temperature table */
	upper = 300;		/* upper limit */
	step = 20;		/* step size */

	celsius = lower;
	printf("%7s  %10s\n", "Celsius", "Fahrenheit");
	while (celsius <= upper) {
		fahr = (9.0 / 5.0) * celsius + 32.0;
		printf("%7.0f  %10.1f\n", celsius, fahr);
		celsius = celsius + step;
	}

	return 0;
}
