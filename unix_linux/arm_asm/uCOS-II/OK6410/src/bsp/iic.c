/*******************************************************************************
* File: iic.c
*
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-30
*******************************************************************************/

/*

IICCON - control register
IICSTAT - control/status register
IICDS - Tx/Rx data shift register
TTCADD - address register

I2C SCL[1](RXD3) -> GPB2
I2C SDA[1](TXD3) -> GPB3

I2C SCL[0] -> GPB5
I2C SDA[0] -> GPB6

INT_I2C0 -> VIC1 No.50
INT_I2C1 -> VIC0 No.5

*/




void init_iic(void)
{
    // use I2C 0 -> set GPB5 & GPB6 - 0010 0010
    GPBCON &= ~(0xFF << 20);
    GPBCON |= (0x22 << 20);
    
    // IICCON:
    // [7] - enable ACK
    // [6] - IICCLK = PCLK / 16 = 66 MHz / 16 = 4.125 MHz
    // [5] - interrupt enable
    // [3:0] - Tx clock = 4.125 MHz / (9 + 1) = 4125 KHz
    IICCON = (1 << 7) | (0 << 6) | (1 << 5) | (0 << 4) | (9 << 0);
    
    // IICSTAT:
    // [7:6] - Master transmit mode
    // [4] - enable Rx/Tx
    
    
    // enable interrupt
    // No.50 - 32 = 18
    VIC1INTENABLE |= (1 << 18);
    VIC1VECTADDR[23] = (unsigned int) iic_isr;
    
}

void iic_write()
{

}





