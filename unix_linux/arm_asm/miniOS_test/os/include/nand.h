/*******************************************************************************
* File: nand.h
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-29
*******************************************************************************/

#ifndef NAND_H
#define NAND_H

#include "registers.h"



#ifndef PAGE_SIZE
#   define PAGE_SIZE 4096
#endif



/* function prorotype */

void init_nand(void);
void nand_reset(void);

void copy_nand_to_sdram(unsigned int start_addr,
                        unsigned int *dest_addr, unsigned int length);

                        
void nand_read_ID(void);
void nand_read(unsigned int start_addr, unsigned int end_addr);

void nand_erase_block(unsigned int start);
void write_to_boot(unsigned int dest, unsigned int length);
void write_to_nand(unsigned int src, unsigned int dest, unsigned int length);


#endif