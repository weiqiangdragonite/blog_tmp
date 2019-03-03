

//#include "lib.h"
extern void leds_on(void);
extern void leds_off(void);
extern void sleep(unsigned int time);

int main(int argc, char *argv[])
{
    while (1) {
        leds_on();
        sleep(100);
        leds_off();
        sleep(100);
    }
    
    return 0;
}