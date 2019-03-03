;**************************************************************************
;                                                                         *
;   PROJECT     : ARM port for UCOS-II                                    *
;                                                                         *
;   MODULE      : MMU.s                                                   *
;                                                                         *
;   AUTHOR      : Michael Anburaj                                         *
;                 URL  : http://geocities.com/michaelanburaj/             *
;                 EMAIL: michaelanburaj@hotmail.com                       *
;                                                                         *
;   PROCESSOR   : EP7312 (32 bit ARM720T RISC core from CIRRUS Logic)     *
;                                                                         *
;   IDE         : SDT 2.51 & ADS 1.2                                      *
;                                                                         *
;   DESCRIPTION :                                                         *
;   EP7312 processor MMU Page table file.                                 *
;                                                                         *
;*************************************************************************/


;*****************************************************************************
;
; The MMU page table.  The virtual memory map created by this page table
; is as follows:
;
; 0x0000.0000 - 0x00ff.ffff (0x0000.0000 - 0x00ff.ffff) => 16M of FLASH ROM (nCS0)
; 0x1000.0000 - 0x10ff.ffff (0x0100.0000 - 0x01ff.ffff) => 1b of NAND FLASH (nCS1)
; 0x2000.0000 - 0x20ff.ffff (0x0200.0000 - 0x02ff.ffff) => 16b Ethernet (nCS2)
; 0x3000.0000 - 0x30ff.ffff (0x0300.0000 - 0x03ff.ffff) => 16b Ethernet (nCS2)
; 0x4000.0000 - 0x40ff.ffff (0x0400.0000 - 0x04ff.ffff) => 16b Ethernet (nCS2)
; 0x5000.0000 - 0x50ff.ffff (0x0500.0000 - 0x05ff.ffff) => 16b Ethernet (nCS2)
; 0x6000.0000 - 0x60ff.ffff (0x0600.0000 - 0x06ff.ffff) => 48K of internal SRAM (nCS6)
; 0x7002.c000 - 0x70ff.ffff (0x0700.0000 - 0x07ff.ffff) => (nCS7)
; 0x8002.c000 - 0x80ff.ffff (0x0800.0000 - 0x08ff.ffff) => EP7209 internal registers (nCS7)
;  Note: This address range should lie in a 256M boundary, since it is mostly used as
;  LCD Frame buffer
; 0xc000.0000 - 0xc0ff.ffff (0x0900.0000 - 0x09ff.ffff) => 16M of SDRAM (nCS8)
;
;*****************************************************************************

;*****************************************************************************
;
; PageTable is the level 1 page table.  Could go up to 4K entries each resolving
; 1 Meg physical memory. Only few are populated since that is all we use. 
; Unpredictable results will occur if the program inadvertently accesses a 
; memory location past the populated entries.
;
;*****************************************************************************

        AREA    |Assembly$$PageTable_|, DATA, READONLY, ALIGN=14

        GBLA    XCount

        EXPORT  PageTable

PageTable
;
; 16M of FLASH ROM, rw, cached
; define addresses 0x0000.0000 - 0x00ff.ffff
;
XCount SETA 0x00000c1a
        WHILE   XCount < 0x01000000
        DCD     XCount
XCount SETA XCount + 0x00100000
        WEND

;
; rw, cached, write buffered
; define addresses 0x0100.0000 - 0x01ff.ffff
;
XCount SETA 0x10000c1e
        WHILE   XCount < 0x11000000
        DCD     XCount
XCount SETA XCount + 0x00100000
        WEND

;
; rw
; define addresses 0x0200.0000 - 0x02ff.ffff
;
XCount SETA 0x20000c12
        WHILE   XCount < 0x21000000
        DCD     XCount
XCount SETA XCount + 0x00100000
        WEND

;
; rw, cached, write buffered
; define addresses 0x0300.0000 - 0x03ff.ffff
;
XCount SETA 0x30000c1e
        WHILE   XCount < 0x31000000
        DCD     XCount
XCount SETA XCount + 0x00100000
        WEND

;
; rw, cached, write buffered
; define addresses 0x0400.0000 - 0x04ff.ffff
;
XCount SETA 0x40000c1e
        WHILE   XCount < 0x41000000
        DCD     XCount
XCount SETA XCount + 0x00100000
        WEND

;
; rw, cached, write buffered
; define addresses 0x0500.0000 - 0x05ff.ffff
;
XCount SETA 0x50000c1e
        WHILE   XCount < 0x51000000
        DCD     XCount
XCount SETA XCount + 0x00100000
        WEND

;
; rw, cached, write buffered
; define addresses 0x0600.0000 - 0x06ff.ffff
;
XCount SETA 0x60000c1e
        WHILE   XCount < 0x61000000
        DCD     XCount
XCount SETA XCount + 0x00100000
        WEND

;
; rw, cached, write buffered
; define addresses 0x0700.0000 - 0x07ff.ffff
;
XCount SETA 0x70000c1e
        WHILE   XCount < 0x71000000
        DCD     XCount
XCount SETA XCount + 0x00100000
        WEND

;
; rw
; define addresses 0x0800.0000 - 0x08ff.ffff
;
XCount SETA 0x80000c12
        WHILE   XCount < 0x81000000
        DCD     XCount
XCount SETA XCount + 0x00100000
        WEND

;
; rw, cached, write buffered
; define addresses 0x0900.0000 - 0x09ff.ffff
;
XCount SETA 0xc0000c1e
        WHILE   XCount < 0xc1000000
        DCD     XCount
XCount SETA XCount + 0x00100000
        WEND


;*****************************************************************************
;
; Level2 is the level 2 page table. Each entry resolves 4 K physical memory.
; This is left empty since all the addresses are resolved by the sector table
;
;*****************************************************************************

;    AREA    |Assembly$$PageTable|, DATA, READONLY, ALIGN=10

;Level2

    END
