/*
 * File: cards.c
 */

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char card_name[3];
    puts("Enter the card name: ");
    scanf("%2s", card_name);
    int val = 0;
    
    if (card_name[0] == 'K') {
        val = 10;
    } else if (card_name[0] == 'Q') {
        val = 10;
    } else if (card_name[0] == 'J') {
        val = 10;
    } else if (card_name[0] == 'A') {
        val = 11;
    } else {
        // atoi: convert a string to int(if string is not num,
        // then the return value is 0)
        val = atoi(card_name);

    }
    
    
    // check if the value is 3 to 6
    if ((val > 2) && (val < 7)) {
        puts("Count has gone up");

    // else check if the card is 10
    } else if (val == 10) {
        puts("Count has gone down");
    }

    printf("The card value is: %i\n", val);
    
    return 0;
}
