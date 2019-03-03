/*******************************************************************************
* File: includes.h
*
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-15
*******************************************************************************/

#include "../app/includes.h"


int update_program(void);


int boot(void)
{
    // init uart
    init_uart();
    
    
    while (1) {
        uart_puts("\n");
        uart_puts("***** Bootloader Menu *****");
        uart_puts("[a] Update program");
        uart_puts("[b] Reboot");
        uart_puts("[c] Run uC/OS-II");
        uart_puts("[d] ATK-HC05 settings");
        uart_puts("[e] test mode");
        uart_puts("***************************");
        
        uart_printf(" >> ");
        char ch = uart_getc();
        uart_puts("");
        
        switch (ch) {
            case 'a':
                update_program();
                break;
            case 'b':
                reboot();
                uart_puts("Rebooting ...");
                while (1);
            case 'c':
                return 0;
            case 'd':
                set_hc05();
                break;
            case 'e':
                hc05_test();
            default:
                break;
        }
    }
    
    return 0;
}


// call from boot() in SYS mode with disable FIQ and IRQ
int update_program(void)
{
    unsigned int addr = 0x5FC00000;
    unsigned char *buffer = (unsigned char *) addr;
    unsigned int wait_time = 0;
    int start = 0;
    int length = 0;
    int size = 0;
    
    uart_puts("Please send binary file!");
    
#define USE_QT_COM

#ifdef USE_QT_COM
    size = uart_get_file_size();
    uart_printf("file size = ");
    uart_print_int(size);
    uart_puts(" bytes\nReceiving binary file ...");
#endif
    
    // get file
    while (1) {
        //
        if (uart_get_file(&buffer[length])) {
            // has receive file
            start = 1;
            wait_time = 0;
            ++length;
        } else {
            if (start) ++wait_time;
        }
        
#ifdef USE_QT_COM
        if (length == size) break;
#endif
        if (wait_time == 0x00FFFFFF) break;
    }
    
    if (length == 0) {
        uart_puts("No file receive.");
        return -1;
    }
    
    uart_printf("\nReceived ");
    uart_print_int(length);
    uart_printf(" bytes data.\n");
    
    
    //
    uart_printf("\nThe first 16 byte are:");
    sdram_read(addr, addr + 0xF);
    uart_printf("\nThe last 16 bytes are:");
    sdram_read(addr + length - 1 - 0xF, addr + length - 1);
    
    //
    uart_puts("\nDo you sure update this program? (Y/n)");
    uart_printf(" >> ");
    char ch = uart_getc();
    uart_puts("");
    
    if (ch != 'Y' && ch != 'y') {
        uart_puts("You have cancel update.");
        return -1;
    }
    
    uart_puts("Updating ...");
    
    // erase block 0 (128 pages, 128 * 4 = 512 K)
    nand_erase_block(0);
    write_to_boot(addr, length);
    
    uart_puts("Update done!");
    
    return 0;
}

/******************************************************************************/

// http://blog.chinaunix.net/uid-26833883-id-3282967.html
int raise(int signum)
{
    signum += 0;
    return 0;
}
