

#define GPMCON      *((volatile unsigned int *) (0xA0008820))   // 虚拟地址
#define GPMDAT      *((volatile unsigned int *) (0xA0008824))   // 虚拟地址

void delay(void);

int main(void)
{
    GPMCON = 0x1111;
    int i = 0;
    
    while (1) {
        GPMDAT = ~(1 << i++);
        delay();
        if (i > 3) i = 0;
    }
    
    return 0;
}

void delay(void)
{
    volatile int i = 0x1000000;
    while (--i);
}