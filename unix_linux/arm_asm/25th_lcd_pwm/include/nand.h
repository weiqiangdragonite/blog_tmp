extern int nand_read(unsigned int nand_start, unsigned int ddr_start, unsigned int len);
extern void nand_erase_block(unsigned long addr);
extern int nand_write(unsigned int nand_start, unsigned char * buf, unsigned int len);


