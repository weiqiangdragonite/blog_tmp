
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
    
    /********************************************************************/
    
    /*
    // test mmu
    unsigned int *pa = (unsigned int *) 0x50A00000;
    *pa = 0xABCDEF;
    uart_printf("\nPA = ");
    uart_print_hex(*pa);
    unsigned int *va = (unsigned int *) 0x02000000;
    uart_printf(", VA = ");
    uart_print_hex(*va);
    uart_puts("");
    */
    
    /*
    int pid = 1;
    char *parameters = "./main.c hello world";
    
    unsigned int addr = (pid - 1) * 0x00200000 + 0x50A00000;
    unsigned int *ptr = (unsigned int *) addr;
    uart_printf("process in: ");
    uart_print_hex((unsigned int) ptr);
    uart_puts("");
    
    // 将进程执行空间清零
    for (unsigned int i = 0, n = addr + 0x00200000; i < n; ++i) {
        ptr[i] = 0;
    }
    
    // 程序参数保存在新进程空间最高1KB空间内，以下简称参数空间
    ptr = (unsigned int *) (addr + 0x00200000 - 1024);
    uart_printf("para in: ");
    uart_print_hex((unsigned int) ptr);
    uart_puts("");
    
    // 根据parameters，计算出程序参数个数，以空格分开
    // ./xxx xxx xxx
    char *argv;
    int argc = 0;
    for (int i = 0; parameters[i] != '\0'; ++i) {
        if (parameters[i] == ' ') ++argc;
    }
    ++argc;
    uart_printf("argc = ");
    uart_send_int(argc);
    uart_puts("");
    
    
    // 将进程pid号保存到参数空间第一个位置
    *ptr++ = pid;
    // 将参数个数保存到参数空间第二个位置
    *ptr++ = argc;
    
    // argv用来存放每个参数字符串，首先跳过argc个位置
    // 每个位置用来放参数字符串指针
    argv = (char *) (ptr + argc);
    uart_printf("argv start addr = ");
    uart_print_hex((unsigned int) argv);
    uart_puts("\n");
    
    // 拷贝每个参数字符串
    for (int i = 0, j = 0; i < argc; ++i, ++j) {
        uart_printf("ptr addr = ");
        uart_print_hex((unsigned int) ptr);
        uart_puts("");
        
        // 将每一个参数字符串地址保存到ptr指向的参数空间
        *ptr = (unsigned int) argv;
        uart_printf("ptr point to = ");
        uart_print_hex((unsigned int) *ptr);
        uart_puts("");
        
        for (; parameters[j] != '\0' && parameters[j] != ' '; ++j) {
            *argv++ = parameters[j];
        }
        // 最后加上字符串结束符
        *argv++ = '\0';
        // 保证每个参数开始位置是4字节对齐
        argv = (char *) (((unsigned int) argv + 4) & (~0x3));
        
        uart_printf("argv = ");
        uart_printf((char *) *ptr);
        uart_puts("");
        
        ptr++;
    }
    */
    
    
    /*****************************************************************/
    
    
    __asm__ (
        "msr cpsr_c, #0x1F\n"   // set to SYS mode, and enable FIQ & IRQ
        "msr cpsr_c, #0x10\n"   // set to USR mode
        :
        :
    );
    
    
    
    while (1) {
        uart_puts("\n");
        uart_puts("*******************");
        uart_puts("[a] Update program");
        uart_puts("[b] Create new process");
        uart_puts("[c] leds on");
        uart_puts("[d] leds off");
        uart_puts("[y] Check CPU mode");
        uart_puts("[z] Reboot");
        uart_puts("*******************");
        
        uart_printf(" >> ");
        char ch = uart_getc();
        //if (ch >= 'a' && ch <= 'z') uart_putc(ch);
        uart_puts("");
        
        char para[80];
        int mode;
        
        switch (ch) {
            case 'a':
                __asm__ (
                    "swi 6\n"
                );
                break;
            case 'b':
                //
                uart_printf(" >> ");
                uart_gets(para);
                
                __asm__ (
                    "mov r0, %0\n"
                    "swi 7\n"
                    :
                    : "r"(para)
                );
                
                break;
            case 'c':
                __asm__ (
                    "swi 3\n"
                );
                break;
            case 'd':
                __asm__ (
                    "swi 4\n"
                );
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
                __asm__ (
                    "msr cpsr_c, #0xDF\n"   // set to SYS mode
                    "mov pc, #0\n"    // restart from pc=0
                //    :   // no output
                //    :   // no input
                );
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
    __asm__ (
        "msr cpsr_c, #0xD3\n"   // disable IRQ/FIQ
    );
    
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
    
    
    
    __asm__ (
        "msr cpsr_c, #0x13\n"   // enable IRQ/FIQ
    );
    
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
