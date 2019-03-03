#include <stdio.h>
#include <limits.h>
#include <float.h>

int main(void)
{
    printf("The size of char is %i\n", sizeof(char));
    printf("The size of int is %i\n", sizeof(int));
    printf("The size of short is %i\n", sizeof(short));
    printf("The size of long is %i\n", sizeof(long));
    printf("The size of unsigned int is %i\n", sizeof(unsigned int));
    printf("The size of unsigned char is %i\n", sizeof(unsigned char));
    printf("The size of long int is %i\n", sizeof(long int));
    printf("The size of long long is %i\n", sizeof(long long));
    printf("The size of float is %i\n", sizeof(float));
    printf("The size of double is %i\n", sizeof(double));
    printf("The size of long double is %i\n", sizeof(long double));

    // use INT, CHAR, SHRT, LNG to replace
    printf("The value of INT_MAX is %i\n", INT_MAX);
    printf("The value of INT_MIN is %i\n", INT_MIN);
    printf("An int takes %i bytes\n", sizeof(int));

    // use FLT, DBL to replace
    printf("The value of FLT_MAX is %f\n", FLT_MAX);
    printf("The value of FLT_MIN is %.50f\n", FLT_MIN);
    printf("A float takes %i bytes\n", sizeof(float));

    return 0;
}
