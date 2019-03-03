/*******************************************************************************
* Projrct: 
*
* Description:
*
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-16
*******************************************************************************/


#include "registers.h"
#include "uart.h"
#include "lcd.h"


/* function prototype */



/* main function */
int main(void)
{
	// init UART
	initUART();
    
    // init LCD
    initLCD();
    
    char input;
    while (1) {
        uartPuts("----------------------------------------\n");
        uartPuts("This program test the LCD!\n");
        uartPuts("Please choose:\n");
        uartPuts("-[a] Enable LCD Power\n");
        uartPuts("-[b] Disable LCD Power\n");
        uartPuts("-[c] Enable LCD BackLight\n");
        uartPuts("-[d] Disable LCD BackLight\n");
        uartPuts("-[e] Enable LCD Display\n");
        uartPuts("-[f] Disable LCD Display\n");
        uartPuts("-[g] Change LCD Display Color\n");
        uartPuts("----------------------------------------\n");
        uartPuts(" >> ");
        
        input = uartGetc();
        uartPutc(input);
        uartPuts("\n\n");
        
        //
        switch (input) {
            case 'a':
                enableLCDPower();
                break;
            case 'b':
                disableLCDPower();
                break;
            case 'c':
                enableLCDBackLight();
                break;
            case 'd':
                disableLCDBackLight();
                break;
            case 'e':
                enableLCDDisplay();
                break;
            case 'f':
                disableLCDDisplay();
                break;
            case 'g':
                changeDisplayColor();
                break;
            default:
                break;
        }
    }
	
	return 0;
}