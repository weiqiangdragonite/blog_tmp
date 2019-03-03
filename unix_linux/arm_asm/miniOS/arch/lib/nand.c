/*******************************************************************************
* File: nand.c
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-27
*******************************************************************************/

/*

Nandflash:ChipType= MLC  ChipName=samsung-K9LBG08U0D
NandFlash:name=NAND 4GiB 1,8V 8-bit,id=d7, pagesize=4096 ,
chipsize=2048 MB,erasesize=524288 oobsize=128
sector size 512 KiB

6410是从nand flash的前四页，每页读2KB共8KB到steppingstone中运行

nand:
        2K      2K
    +-------+-------+
  0 |   A   |   B   |
    +-------+-------+
  1 |   B   |   C   |
    +-------+-------+
  2 |   C   |   D   |
    +-------+-------+
  3 |   D   |   E   |
    +-------+-------+
  4 |   E   |   F   |
    +-------+-------+
  5 |   G   |   H   |
    +-------+-------+
    |      ...      |
    +---------------+


以page（页）为单位进行读写，以block（块）为单位进行擦除。每一页中又分为
main区和spare区，main区用于正常数据的存储，spare区用于存储一些附加信息，
如块好坏的标记、块的逻辑地址、页内数据的ECC校验和等。(4K + 218 Byte)

K9GAG08X0D:
一页(page): 4K + 218
一块(block): (4K + 218) * 128 pages = 512K + 27.25K
整个设备(device): (4K + 218) * 128 pages * 4096 Blocks
                  = (512K + 27.25K) * 4096
                  = 2157 MB = 2157 * 8 = 17256 Mbits
                  
main容量为: 512K * 4096 = 2048 M
即总共有2*1024*1024=524288 pages


如何知道我自己芯片的block大小啊
page - 4(K)
block - 128 pages * 4(K) = 512(K)
device - 8192 blocks * 512(K) = 4096(MB)

一个block有128 page，擦除时是擦除一个block，即擦除第0 page ~ 第127 page。
现擦除第二个block，即第128 page开始:
int page = start / 4096;
所以start = 0x80000
因为是擦除第128 page ~ 第255 page，所以读取第127、128、129、254、255、256 page的数据，
如果第128、129、254、255 page的数据全为F，第256 page的数据不全为F，则可判断应该
是1个block含128 pages。



5个周期:
1st: A0 ~ A7
2nd: A8 ~ A12
3rd: A13 ~ A20
4th: A21 ~ A28
5th: A29 ~ A31


首先需要写入读ID命令(0x90)，然后再写入0x00地址，并等待芯片就绪，
就可以读取到一共五个周期的芯片ID，第一个周期为厂商ID，第二个周期为设备ID，
第三个周期至第五个周期包括了一些具体的该芯片信息。

Nand Flash的每一页有两区：main区和spare区，main区用于存储正常的数据，
spare区用于存储其他附加信息，其中就包括ECC校验码。当我们在写入数据的时候，
我们就计算这一页数据的ECC校验码，然后把校验码存储到spare区的特定位置中，
在下次读取这一页数据的时候，同样我们也计算ECC校验码，
然后与spare区中的ECC校验码比较，如果一致则说明读取的数据正确，
如果不一致则不正确。ECC的算法较为复杂，好在S3C2440能够硬件产生ECC校验码，
这样就省去了不少的麻烦事。S3C2440既可以产生main区的ECC校验码，
也可以产生spare区的ECC校验码。因为K9F2G08U0A是8位IO口，因此S3C2440
共产生4个字节的main区ECC码和2个字节的spare区ECC码。在这里我们规定，
在每一页的spare区的第0个地址到第3个地址存储main区ECC，
第4个地址和第5个地址存储spare区ECC。

产生ECC校验码的过程为：在读取或写入哪个区的数据之前，先解锁该区的ECC，
以便产生该区的ECC。在读取或写入完数据之后，再锁定该区的ECC，
这样系统就会把产生的ECC码保存到相应的寄存器中。

main区的ECC保存到NFMECC0/1中（因为K9F2G08U0A是8位IO口，
因此这里只用到了NFMECC0），spare区的ECC保存到NFSECC中。

对于读操作来说，我们还要继续读取spare区的相应地址内容，
以得到上次写操作时所存储的main区和spare区的ECC，
并把这些数据分别放入NFMECCD0/1和NFSECCD的相应位置中。
最后我们就可以通过读取NFESTAT0/1（因为K9F2G08U0A是8位IO口，
因此这里只用到了NFESTAT0）中的低4位来判断读取的数据是否正确，
其中第0位和第1位为main区指示错误，第2位和第3位为spare区指示错误。



http://blog.csdn.net/cp1300/article/details/7769100
http://blog.csdn.net/canjiangsu/article/details/6162677
http://blog.csdn.net/girlkoo/article/details/8115849

*/


#include "registers.h"
#include "uart.h"
#include "nand.h"


void init_nand(void)
{
    // 1. set MEM_SYS_CFG[5:0] to NFCON
    MEM_SYS_CFG = 0;
    
    // 2. set the NFCONF register
    // TACLS = 0; TWRPH0 = 3; TWRPH1 = 1
    // MsgLength = 512-byte, ECCType = 4-bit ECC
    NFCONF = (NFCONF & 0xBC7F888F) | 0x01000310;
    
    // 3. set the NFCONT and enable NAND Flash controller
    // disable lock-tight and soft lock
    // init main aera ECC value, clear MainECCLock
    //   _XX00_XXXX_XXXX_1111_X111
    NFCONT = (NFCONT & 0xFFFCFF08) | 0xF7;
    
    nand_reset();
    
    return;
}

void nand_reset(void)
{
    // 1. enable the nand chip select
    NFCONT &= ~2;
    
    // 2. RnB transition is not detected
    //NFSTAT &= ~(1 << 4);
    
    // 3. send the reset command
    NFCMMD = 0xFF;
    
    // 4. wait for ready
    while (1) {
        if (NFSTAT & 1) break;
    }
    
    // 5. disable the nand chip select
    NFCONT |= 2;
    
    return;
}


void copy_nand_to_sdram(unsigned int start_addr,
                        unsigned int *dest_addr, unsigned int length)
{
    
    unsigned int count = 0;
    unsigned int page = 0;
    unsigned int column = 0;
    unsigned int size = 0;
    
    page = start_addr / PAGE_SIZE;
    column = start_addr - page * PAGE_SIZE;
    
    // enable the nand chip select
    NFCONT &= ~2;
    
    // begin to copy data
    while (1) {
        size = PAGE_SIZE;
        if (page >= 0 && page < 4) size = 2048;
        if (column >= size) ++page;

        // send the 1st READ command
        NFCMMD = 0;
        
        // send the column address
        NFADDR = column & 0xFF;
        NFADDR = (column >> 8) & 0x1F;
        // send the page address
        NFADDR = page & 0xFF;
        NFADDR = (page >> 8) & 0xFF;
        NFADDR = (page >> 16) & 0xFF;
        
        // send the 2nd READ command
        NFCMMD = 0x30;
        
        // wait for nand ready
        while (1) {
            if (NFSTAT & 1) break;
        }
        
        // start to read data
        // read one page(4 KB) data, every time read 4 bytes
        for (int i = column; i < size && count < length; i += 4) {
            *dest_addr++ = NFDATA;
            count += 4;
        }
        
        // 
        if (count >= length) break;
        
        //
        ++page;
        column = 0;
        
    }
    
    // disable the nand chip select
    NFCONT |= 2;
    
    return;
}


void nand_read_ID(void)
{
    // 1. enable the nand chip select
    NFCONT &= ~2;
    
    // 2. send Read ID command
    NFCMMD = 0x90;
    
    // 3. send address
    NFADDR = 0;
    
    // 4. wait for nand ready
    while (1) {
        if (NFSTAT & 1) break;
    }
    
    // 5. read data
    // we just need 6 bytes to store the nand ID
    // 厂商代码
    // 设备代码
    unsigned int data[3];
    data[0] = NFDATA;
    data[1] = NFDATA;
    data[2] = NFDATA;
    
    // 6. disable the nand chip select
    NFCONT |= 2;
    
    //
    uart_printf("The ID is: ");
    uart_print_hex(data[2]);
    uart_print_hex(data[1]);
    uart_print_hex(data[0]);
    uart_puts("");
    
    return;
}


void nand_read(unsigned int start_addr, unsigned int end_addr)
{
    unsigned int length = end_addr - start_addr;
    unsigned int count = 0;
    unsigned int page = 0;
    unsigned int column = 0;
    unsigned int data = 0;
    
    // enable the nand chip select
    NFCONT &= ~2;
    
    while (1) {
        page = start_addr / PAGE_SIZE;
        column = start_addr - page * PAGE_SIZE;
        
        // print some msg
        uart_puts("");
        uart_printf("start_addr = ");
        uart_print_hex(start_addr);
        uart_printf("  page = ");
        uart_print_hex(page);
        uart_printf("  column = ");
        uart_print_hex(column);
        uart_puts("");
        
        // send the 1st READ page command
        NFCMMD = 0;
        
        // send the column address
        NFADDR = column & 0xFF;
        NFADDR = (column >> 8) & 0x1F;
        // send the page address
        NFADDR = page & 0xFF;
        NFADDR = (page >> 8) & 0xFF;
        NFADDR = (page >> 16) & 0xFF;
        
        // send the 2nd READ page command
        NFCMMD = 0x30;
        
        // wait for nand ready
        while (1) {
            if (NFSTAT & 1) break;
        }
        
        // start to read data
        // read one page(4 KB) data, every time read 4 bytes
        for (int i = column, j = 0; i < PAGE_SIZE &&
                                        count < length; i += 4, ++j) {
            data = NFDATA;

            // print the addr, every line form 0 ~ F, 4 column
            if (j % 4 == 0) {
                uart_puts("");
                uart_print_hex(start_addr);
                uart_printf(" : ");
            }
            uart_print_addr(data);
            uart_printf("  ");

            start_addr += 4;
            count += 4;
        }
        uart_puts("");
        
        // check read size
        if (count >= length) break; 
    }
    
    // disable the nand chip select
    NFCONT |= 2;
    
    return;
}


// Block Erase 60h D0h
// 在擦除结束前还要判断是否擦除操作成功，
// 另外同样也存在需要判断是否为坏块以及要标注坏块的问题。
// 一块(block): (4K + 218) * 128 pages = 512K + 27.25K
// rNF_IsBadBlock和rNF_MarkBadBlock
void nand_erase_block(unsigned int start)
{
    // check the block if is bad block
    
    // enable the nand chip select
    NFCONT &= ~2;
    
    // send the erase block command
    NFCMMD = 0x60;
    
    // send the addr
    int page = start / 4096;
    NFADDR = page & 0xFF;
    NFADDR = (page >> 8) & 0xFF;
    NFADDR = (page >> 16) & 0xFF;
    
    //
    NFCMMD = 0xD0;
    
    // wait
    while (1) {
        if (NFSTAT & 1) break;
    }
    
    // disable the nand chip select
    NFCONT |= 2;
    
    // check
    
    return;
}


void write_to_boot(unsigned int dest, unsigned int length)
{
    unsigned int count = 0;
    unsigned int page = 0;
    unsigned int column = 0;

    // enable the nand chip select
    NFCONT &= ~2;
    
    while (1) {
        
        if (page > 0 && page < 5) dest -= 2048;
        
        uart_printf("\ndest_addr = 0x");
        uart_print_hex(dest);
        uart_printf("  count = ");
        uart_print_int(count);
        uart_puts("");
    
        // send write command
        NFCMMD = 0x80;
    
        // send the column address
        NFADDR = column & 0xFF;
        NFADDR = (column >> 8) & 0x1F;
        // send the page address
        NFADDR = page & 0xFF;
        NFADDR = (page >> 8) & 0xFF;
        NFADDR = (page >> 16) & 7;
    
    
        // write data
        for (int i = column; i < PAGE_SIZE && count < length; i += 4) {
            NFDATA = *(unsigned int *) dest;
            dest += 4;
            if (i < 2048) count += 4;
        }
    
        // 
        NFCMMD = 0x10;
    
        // wait
        while (1) {
            if (NFSTAT & 1) break;
        }
        
        //
        if (count >= length) break;
        ++page;
    
    }
    
    // disable the nand chip select
    NFCONT |= 2;
    
    return;
}



/*

with ECC

void nand_read(unsigned int start_addr, unsigned int end_addr)
{
    //unsigned int start_addr = (unsigned int) start;
    //unsigned int end_addr = (unsigned int) end;
    //unsigned int *dest_addr = (unsigned int *) dest;
    unsigned int data = 0;
    
    
    // reset main area ECC
    NFCONT |= (1 << 5);
    
    // unlock the main area ECC
    NFCONT &= ~(1 << 7);
    
    // 1. enable the nand chip select
    NFCONT &= ~2;
    
    
    // 
#ifndef PAGE_SIZE
#   define PAGE_SIZE 4096
#endif

    
    unsigned int page_num = start_addr / PAGE_SIZE;
    unsigned int page_end = end_addr / PAGE_SIZE;
    // begin to read data
    while (1) {
    
        
        int size = PAGE_SIZE;
        if (page_num >= 0 && page_num < 4) {
            size = 2048;
            start_addr = page_num * PAGE_SIZE;
        }
        
        
        
        // XXX / 4096 = XXX >> 12;
        //int page = start_addr / 4096;
        //int col = start_addr - page * 4096;
        
        uart_puts("");
        uart_printf("start_addr = ");
        uart_print_hex(start_addr);
        uart_printf("  page = ");
        uart_print_hex(page_num);
        uart_puts("");
        
        
        // 2. send the 1st READ page command
        NFCMMD = 0;
        
        // send the column
        //NFADDR = col & 0xFF;
        //NFADDR = (col >> 8) & 0x1F;
        NFADDR = 0;
        NFADDR = 0;
        // send the page
        // page has 19 bits, total 524288 pages
        NFADDR = page_num & 0xFF;
        NFADDR = (page_num >> 8) & 0xFF;
        NFADDR = (page_num >> 16) & 7;
        
        
        // 3. send the start address
        // send A0 ~ A7
        //NFADDR = start_addr & 0xFF;
        // send A8 ~ A12
        //NFADDR = (start_addr >> 8) & 0x1F;
        // send A13 ~ A20
        //NFADDR = (start_addr >> 13) & 0xFF;
        // send A21 ~ A28
        //NFADDR = (start_addr >> 21) & 0xFF;
        // send A29 ~ A31
        //NFADDR = (start_addr >> 29) & 7;
        
        // 4. send the 2nd READ page command
        NFCMMD = 0x30;
        
        // 5. wait for nand ready
        while (1) {
            if (NFSTAT & 1) break;
        }
        
        // 6. start to read data
        // everytime we read one page(4 KB) data, we read 4 bytes
        // every time, so we need to read 1024 times for one page.
        
        int column = 0;
        for (int i = 0; i < size; i += 4) {
            data = NFDATA;

            if (column == 0 || column == 4) {
                uart_puts("");
                uart_print_hex(start_addr);
                uart_printf(" : ");
                column = 0;
            }
            uart_print_addr(data);
            uart_printf("  ");
            ++column;

            start_addr += 4;
            // we had read enough data, and then break the while loop
            //if (has_copy >= length) break;
        }
        
        uart_puts("");
        
        uart_puts("ECC check: ");
        
        
        // lock the main area ECC
        NFCONT |= (1 << 7);
        
        // unlock the spare area ECC
        NFCONT &= ~(1 << 6);
        
        // get the main area ECC
        unsigned int ecc = NFDATA;
        uart_print_hex(ecc);
        uart_printf(" - ");
        
        // put the main area ECC parity code to NFMECCD0/1
        NFMECCD0 = (ecc & 0xFF) | ((ecc & 0xFF00) << 8);
        NFMECCD1 = ((ecc & 0x00FF0000) >> 16) | ((ecc & 0xFF000000) >> 8);
        
        // lock the spare area ECC
        NFCONT |= (1 << 6);
        
        // get the spare area ECC (just first two bytes)
        ecc = NFDATA;
        uart_print_hex(ecc);
        
        // put the spare area ECC parity code to NFSECCD
        NFSECCD = (ecc & 0xFF) | ((ecc & 0xFF00) << 8);
        
        // check the 
        while (1) {
            if (NFSTAT & (1 << 6)) break;
        }
        uart_printf(" - MECC Error: ");
        uart_print_hex(NFECCERR0);
        uart_puts("");
        
        
        // check
        if (page_num >= page_end) break;
        ++page_num;
        
        // we havent read enough data, and we continue.
        // add the start_addr with one page length,
        // because we have read one page data already.
        //start_addr += 4096;
        
    }
    
    // 7. disable the nand chip select
    NFCONT |= 2;
    
    return;
}

*/






























