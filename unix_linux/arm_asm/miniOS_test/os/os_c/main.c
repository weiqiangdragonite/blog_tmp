
#include "registers.h"
#include "uart.h"
#include "nand.h"
#include "mmu.h"
#include "key.h"
#include "led.h"
#include "buzzer.h"
#include "time.h"
#include "schedule.h"



int update_program(void);
void copy_program(unsigned int *dest);
void sdram_read(unsigned int start, unsigned int end);
void test_nand(void);
void read_nand(void);
void reboot();

extern int create_process(char *para);
extern void OS_ENTER_CRITICAL(void);
extern void OS_EXIT_CRITICAL(void);
extern void leds_on(void);
extern void leds_off(void);

int main(void) {

    // create page table
    create_page_table();
    
    // open mmu
    enable_mmu();

    // init uart
    init_uart();
    
    // init keys
    init_key();
    
    // init leds
    init_led();
    
    // init buzzer
    init_buzzer();
    
    // init pwm
    init_pwm();
    
    //
    init_schedule();
    
    
    
    __asm__ (
        "msr cpsr_c, #0x1F\n"   // set to SYS mode, and enable FIQ & IRQ
        "msr cpsr_c, #0x10\n"   // set to USR mode
    );
    
    
    
    while (1) {
        uart_puts("\n");
        uart_puts("*******************");
        uart_puts("[a] Update program");
        uart_puts("[b] Create new process");
        uart_puts("[c] leds on");
        uart_puts("[d] leds off");
        uart_puts("[w] read nand");
        uart_puts("[x] test nand");
        uart_puts("[y] Check CPU mode");
        uart_puts("[z] Reboot");
        uart_puts("*******************");
        
        uart_printf(" >> ");
        char ch = uart_getc();
        uart_puts("");
        
        char para[80];
        int mode;
        
        switch (ch) {
            case 'a':
                update_program();
                break;
            case 'b':
                //
                uart_printf(" >> ");
                uart_gets(para);
                
                create_process(para);
                
                break;
            case 'c':
                leds_on();
                break;
            case 'd':
                leds_off();
                break;
            case 'w':
                read_nand();
                break;
            case 'x':
                test_nand();
                break;
            case 'y':
                __asm__ (
                    "mrs r0, cpsr\n"
                    "mov %0, r0\n"
                    : "=r" (mode)   // output
                    : // input
                );
                uart_printf("\ncpsr = ");
                uart_print_hex(mode);
                break;
            case 'z':
                //
                reboot();
                break;
            default:
                break;
        }
    }
    
    return 0;

}




int update_program(void)
{

    //
    OS_ENTER_CRITICAL();
    
    
    unsigned int addr = 0x5FC00000;
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
        return -1;
    }
    
    uart_printf("\nReceive ");
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
    
    // erase block 0 (128 pages)
    nand_erase_block(0);
    write_to_boot(addr, length);
    
    uart_puts("Update done!");
    
    
    uart_puts("\nDo you want to show the content? (Y/n)");
    uart_printf(" >> ");
    ch = uart_getc();
    if (ch == 'Y' || ch == 'y') nand_read(0, 0x5000);
    
    
    
    //__asm__ (
    //    "msr cpsr_c, #0x13\n"   // enable IRQ/FIQ
    //);
    
    OS_EXIT_CRITICAL();
    
    return 0;
}

// 
void copy_program(unsigned int *dest)
{
    unsigned int addr = 0x5FC00000;
    unsigned char *buffer = (unsigned char *) addr;
    unsigned int wait_time = 0;
    int start = 0;
    int length = 0;
    
    uart_puts("please send your program! (wait for a while)");
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
        uart_puts("No program receive.");
        return;
    }
    
    uart_printf("\nReceive ");
    uart_print_int(length);
    uart_printf(" bytes data.\n");
    
    
    //
    uart_printf("\nThe first 16 byte are:");
    sdram_read(addr, addr + 0xF);
    uart_printf("\nThe last 16 bytes are:");
    sdram_read(addr + length - 1 - 0xF, addr + length - 1);
    
    //
    uart_puts("\nDo you sure run this program? (Y/n)");
    uart_printf(" >> ");
    char ch = uart_getc();
    uart_puts("");
    
    if (ch != 'Y' && ch != 'y') {
        uart_puts("You have cancel update.");
        return;
    }
    
    uart_puts("Copying to ram ...");
    
    // copy code to ram
    unsigned int *src = (unsigned int *) addr;
    unsigned int has_copy = 0;
    while (1) {
        if (has_copy >= length) break;
        *dest++ = *src++;
        has_copy += 4;
    }
    
    //
    
    return;
}

void test_nand(void)
{
    // erase block
    unsigned int start = (unsigned int) 1048575 * 4096;
    nand_erase_block(start);
    
    //
    write_to_nand(0x50000000, start, 4096);
    
    
    return;
}

void read_nand(void)
{
    //
    int page;
    uart_printf("\nplease input page: ");
    page = uart_get_int();
    uart_puts("");
    
    // 
    nand_read(page * 4096, (page + 1) * 4096);
    
    return;
}
