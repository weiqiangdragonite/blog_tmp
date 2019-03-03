
#include "registers.h"
#include "uart.h"
#include "nand.h"
#include "time.h"

void update_program(void);
void sdram_read(unsigned int start, unsigned int end);

int main(void) {

    init_uart();
    
    GPMCON = (GPMCON & ~0xFFFF) | 0x1111;
    GPMDAT &= ~0xF;
    
    
    while (1) {
        uart_puts("\n");
        uart_puts("*******************");
        uart_puts("[a] Update program");
        uart_puts("[b] Read RTC time");
        uart_puts("[c] Change RTC time");
        uart_puts("[z] Reboot");
        uart_puts("*******************");
        
        uart_printf(" >> ");
        char ch = uart_getc();
        //if (ch >= 'a' && ch <= 'z') uart_putc(ch);
        uart_puts("");
        
        switch (ch) {
            case 'a':
                update_program();
                break;
            case 'b':
                uart_puts("Time is: ");
                read_rtc_time();
                uart_puts("");
                break;
            case 'c':
                change_rtc_time();
                break;
            case 'z':
                __asm__ (
                    "mov pc, #0\n"    // restart from pc=0
                    :   // no output
                    :   // no input
                );
                break;
            default:
                break;
        }
    }
    
    return 0;

}


void update_program(void)
{
    unsigned int addr = 0x55000000;
    unsigned char *buffer = (unsigned char *) addr;
    unsigned int wait_time = 0;
    int start = 0;
    int length = 0;
    
    uart_puts("please send file! (wait for a while)");
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
    
        if (wait_time == 0x00FFFFFF) break;
    }
    
    if (length == 0) {
        uart_puts("No file receive.");
        return;
    }
    
    uart_printf("\nReceive ");
    uart_send_int(length);
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
        return;
    }
    
    uart_puts("Updating ...");
    
    // erase block 0 (128 pages)
    nand_erase_block(0);
    write_to_boot(addr, length);
    
    uart_puts("Update done!");
    
    
    uart_puts("\nDo you want to show the content? (Y/n)");
    uart_printf(" >> ");
    ch = uart_getc();
    if (ch == 'Y' || ch == 'y') nand_read(0, 0x5000);
    
    
    return;
}
