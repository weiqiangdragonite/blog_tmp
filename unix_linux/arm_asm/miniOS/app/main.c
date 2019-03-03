

#include "lib.h"

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