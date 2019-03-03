#include <stdio.h>
#include <string.h>

int main(void)
{
    unsigned int addr = 0x0;
    unsigned int ddr_start = 0x50000000;
    unsigned int ddr_end = 0x51000000;
    unsigned int len = ddr_end - ddr_start;

    unsigned char *dest = (unsigned char *) ddr_start;
    unsigned int *dest1 = &ddr_start;
    unsigned char *dest2 = (unsigned char *) 0x50000000;

    char a = 'a';
    unsigned char b = 'b';
    char *c = &a;
    unsigned char *d = &b;

    printf("addr = %#x\n", addr);
    printf("len = %#x\n", len);
    printf("size of *dest = %i\n", sizeof(dest));
    printf("the address of ddr_start = %#x\n", (unsigned int) (&ddr_start));
    printf("the ascii value of ddr_start = %#x\n", (unsigned int) ddr_start);
    printf("the address of *dest = %#x\n", (unsigned int) (&dest));
    printf("the ascii value of *dest = %#x\n", (unsigned int) dest);
    //printf("the ascii value of *dest point to = %#x\n", (unsigned int) (*dest));
    printf("the address of dest[0] = %#x\n", (unsigned int) (&dest[0]));
    printf("the address of dest[1] = %#x\n", (unsigned int) (&dest[1]));
    printf("the address of dest[0] = %#x\n", (unsigned int) (&(*dest++)));
    printf("the address of dest[1] = %#x\n", (unsigned int) (&(*dest)));

    printf("--------------------------------------------------------\n");
    printf("the address of *dest1 = %#x\n", (unsigned int) (&dest1));
    printf("the ascii value of *dest1 = %#x\n", (unsigned int) dest1);
    printf("the ascii value of *dest1 point to = %#x\n", (unsigned int) (*dest1));
    printf("the address of dest1[0] = %#x\n", (unsigned int) (&dest1[0]));
    printf("the address of dest1[1] = %#x\n", (unsigned int) (&dest1[1]));

    printf("--------------------------------------------------------\n");
    printf("the ascii value of *dest2 = %#x\n", (unsigned int) dest2);

    printf("---------------------------------------------------------\n");
    printf("size of a = %i\n", sizeof(a));
    printf("size of b = %i\n", sizeof(b));
    printf("size of *c = %i\n", sizeof(c));
    printf("size of *d = %d\n", sizeof(d));
    printf("the address of a = %#x\n", (unsigned int) (&a));
    printf("the ascii value of a = %#x\n", (unsigned int) a);
    printf("the address of c = %#x\n", (unsigned int) (&c));
    printf("the ascii value of c = %#x\n", (unsigned int) c);
    printf("the ascii value of c point to = %#x\n", (unsigned int) (*c));
    printf("---------------------------------------------------------\n\n");

    /*************************************************************************/
    
    unsigned char str[] = "hello, world";
    unsigned int str_addr = (unsigned int) (&str);
    //unsigned char *array = str;
    unsigned char *array = (unsigned char *) str_addr;

    printf("the address of str[] = %#x\n", (unsigned int) (&str));
    printf("the str_addr = %#x\n", (unsigned int) str_addr);
    printf("the address of *array = %#x\n", (unsigned int) (&array));
    printf("the ascii value of *array = %#x\n", (unsigned int) array);

    puts("\n");
    for (int i = 0; i < strlen(str); ++i) {
        printf("the address of str[%i] = %#x\n", i, (unsigned int) (&str[i]));
        printf("the ascii  value of str[%i] = %#x\n", i, (unsigned int) str[i]);
    }

    puts("\n");
    for (int i = 0; i < strlen(str); ++i) {
        printf("the address of *array%i = %#x\n", i, (unsigned int) (&array[i]));
        printf("the ascii value of *array%i = %#x\n", i, (unsigned int) array[i]);
    }

    puts("\n");
    unsigned int *array1 = (unsigned int *) str_addr;
    for (int i = 0; i < 4; ++i) {
        printf("the address of *array1[%i] = %#x\n", i, (unsigned int) (&array1[i]));
        printf("the ascii value of *array1[%i] = %#x\n", i, (unsigned int) array1[i]);
    }

    printf("the value = %s\n", (unsigned char *)array1);

    return 0;
}
