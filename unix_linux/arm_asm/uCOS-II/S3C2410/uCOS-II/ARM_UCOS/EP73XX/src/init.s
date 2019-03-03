;**************************************************************************
;                                                                         *
;   PROJECT     : ARM port for UCOS-II                                    *
;                                                                         *
;   MODULE      : INIT.s                                                  *
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
;   EP7312 processor Start up assembly code file.                         *
;                                                                         *
;*************************************************************************/


; The following are conditional assembly flags.  Leave commented if you don't want
; to assemble the corresponding code block.
                                
MMU_enabled  EQU 0x1                ; these can be defined in the project file
DRAM_enabled  EQU 0x1               ; if commented, then not defined by default
;FIQ_enabled  EQU 0x1               ;

        IF :DEF: MMU_enabled
SRAM_SADDR    EQU 0x06000000        ; SRAM starting address
ISR_BADDR     EQU 0x06000000        ; plus 0x20 is the RW base address -linker setting
        ELSE
SRAM_SADDR    EQU 0x60000000        ; SRAM starting address
ISR_BADDR     EQU 0x60000000        ; plus 0x20 is the RW base address -linker setting
        ENDIF

        IF :DEF: debug_build
SFR_BADDR     EQU 0x08000000        ; This base address should be used after MMU mapping
        ELSE
SFR_BADDR     EQU 0x80000000        ; This base address should be used before MMU mapping
        ENDIF

SRAM_SIZE     EQU 48*1024           ; 48K internal SRAM
SRAM_EADDR    EQU SRAM_SADDR+SRAM_SIZE-1 ; SRAM end address

_SVC_STKSIZE  EQU 1024*20
_UND_STKSIZE  EQU 256
_ABT_STKSIZE  EQU 256
_IRQ_STKSIZE  EQU 1024*1
_FIQ_STKSIZE  EQU 256

STK_SIZE      EQU _SVC_STKSIZE+_UND_STKSIZE+_ABT_STKSIZE+_IRQ_STKSIZE+_FIQ_STKSIZE

STK_SADDR     EQU SRAM_EADDR+1-STK_SIZE

; Register definition
rMEMCFG1      EQU SFR_BADDR+0x0180
rMEMCFG2      EQU SFR_BADDR+0x01C0
rINTMR1       EQU SFR_BADDR+0x0280
rINTMR2       EQU SFR_BADDR+0x1280
rINTSR1       EQU SFR_BADDR+0x0240
rINTSR2       EQU SFR_BADDR+0x1240
rSDCONF       EQU SFR_BADDR+0x2300
rSDRFPR       EQU SFR_BADDR+0x2340
rSYSCON2      EQU SFR_BADDR+0x1100
rSYSCON3      EQU SFR_BADDR+0x2200

; Pre-defined constants
USERMODE      EQU 0x10
FIQMODE       EQU 0x11
IRQMODE       EQU 0x12
SVCMODE       EQU 0x13
ABORTMODE     EQU 0x17
UNDEFMODE     EQU 0x1b
MODEMASK      EQU 0x1f
NOINT         EQU 0xc0


        MACRO
$HandlerLabel HANDLER $HandleLabel

$HandlerLabel
        sub sp,sp,#4
        stmfd sp!,{r0}
        ldr r0,=$HandleLabel
        ldr r0,[r0]
        str r0,[sp,#4]
        ldmfd sp!,{r0,pc}
        MEND


        AREA  |Assembly$$code|, CODE, READONLY

        ENTRY

        b ResetHandler              ; for debug
        b HandlerUndef              ; handlerUndef
        b HandlerSWI                ; SWI interrupt handler
        b HandlerPabort             ; handlerPAbort
        b HandlerDabort             ; handlerDAbort
        b .                         ; handlerReserved
        b HandlerIRQ                ; handlerIRQ
        b HandlerFIQ                ; handlerFIQ

HandlerFIQ     HANDLER HandleFIQ
HandlerIRQ     HANDLER HandleIRQ
HandlerUndef   HANDLER HandleUndef
HandlerSWI     HANDLER HandleSWI
HandlerDabort  HANDLER HandleDabort
HandlerPabort  HANDLER HandlePabort


;*****************************************************************************
;
; ResetHandler is the startup code to be used
;
;*****************************************************************************
ResetHandler
        ;
        ; Setup the MMU for 32 bit code and data.
        ;
        IF (:LNOT: :DEF: debug_build)
        ldr r0,=0x00000070
        mcr p15,0,r0,c1,c0,0
        ENDIF

        ldr r0,=0x00000000
        ;        
        ;disable all interrupts (they should be disabled on reset, but just in case...)

        ldr r12,=rINTMR1
        str r0,[r12]                ; INTMR1 = 0x8000.0280
        ldr r12,=rINTMR2
        str r0,[r12]                ; INTMR2 = 0x8000.1280

        IF (:LNOT: :DEF: debug_build)
;
;***************************************************************************************
; Do this section only if DRAM is required.
        IF (:DEF: DRAM_enabled)
        
DRAMControlvalue  EQU 0x00000522    ; CASLAT=2, SDSIZE=64Mb, SDWIDTH=16, CLKCTL=0, SDACTIVE=1
DRAMConfigvalue   EQU 0x00000100    ; REFRATE=7.11uS at 36MHz BCLK

        ; Enable SDRAM Bank 0 and Bank 1 for EP73xx
        ;
        ldr r1,=DRAMControlvalue
        ldr r12,=rSDCONF
        str r1,[r12]                ; store in SDCONF

        ldr r1,=DRAMConfigvalue
        ldr r12,=rSDRFPR
        str r1,[r12]                ; store value in SDRFPR

        ;
        ;        Stub for memory test
        ;
        ;
        ; Set SYSCON2 to zero (default value, step not necessary).  Bit 2 sets x32 DRAM
        ;
        ldr r12,=rSYSCON2
        str r0,[r12]                ; init syscon2 register at 0x8000.1100
;**********************************************************************************************
; End of DRAM initialization
                ENDIF
         
        ;
        ; Set bits 1:2 in SYSCON3 for 74 MHz clock speed (default is 18MHz on reset)
        ;
        ldr r1,=0x06
        ldr r12,=rSYSCON3
        str r1,[r12]                ; init syscon3 register at 0x8000.2200

; Now configure the MemConfig register to get the following:
;
; nCS0 = NOR FLASH, 32-bit, 3 wait states
; nCS1 = NAND FLASH, 32-bit, 2 wait states
; nCS2 = Ethernet, 16-bit, 8 wait states (was 32-bit =0x00)
; nCS3 = Parallel/Keyboard/GPIOs, 32-bit, 1 wait state
; nCS4 = USB, 8-bit, 1 wait state, 2 w/s random (was 32-bit, =0x3c)
; nCS5 = Unused/general purpose, 32-bit, 8 wait states
;
                
MemConfig1value  EQU 0x3d01190c
MemConfig2value  EQU 0x0000013d     ; boot rom and internal SRAM are ignored 
        
        ;
        ; configure nCS0-nCS3
        ;
        ldr r1,=MemConfig1value
        ldr r12,=rMEMCFG1
        str r1,[r12]                ; MEMCFG1 = 0x8000.0180
        ;
        ; configure nCS4 &nCS5
        ;
        ldr r1,=MemConfig2value
        ldr r12,=rMEMCFG2
        str r1,[r12]                ; MEMCFG2 = 0x8000.01c0

        ENDIF                       ; (:LNOT: :DEF: debug_build)

; **************************************************************************
; Define Stacks
; The follow section defines the stack pointer for IRQ and SVC modes.
; This is optional as the debugger will assign it's own stack area with the
; $top_of_memory variable in "debugger internals".
; However, this code is necessary if this program is used to launch an 
; embedded applications in C or assembly.
; **************************************************************************
        ldr sp,=SVCStack            ; Why?

        bl InitStacks

;********************************************************************
; End of Stack Setup
;********************************************************************
        ;
        ;
        IF (:LNOT: :DEF: debug_build)

;********************************************************************
;
; Set up the MMU.  Start by flushing the cache and TLB.
; This section may be eliminated if MMU is not desired.
;********************************************************************
        ;
        IF (:DEF: MMU_enabled)
        ldr r0,=0x00000000
        mcr p15,0,r0,c5,c0
        mcr p15,0,r0,c7,c0

        ;
        ; Set user mode access for all 16 domains.
        ;
        ldr r0,=0x55555555
        mcr p15,0,r0,c3,c0

        ;
        ; Tell the MMU where to find the page table.
        ;
        IMPORT  PageTable
        ldr r0,=PageTable
        mcr p15,0,r0,c2,c0

        ;
        ; Enable the MMU.
        ;
        ldr r0,=0x0000007d
        mcr p15,0,r0,c1,c0
        ;
        ;
        ; There should always be two NOP instructions following the enable or
        ; disable of the MMU.
        ;
        mov r0,r0
        mov r0,r0
;*****************************************************************************
; End of MMU initialization
;*****************************************************************************
        ENDIF                       ; (:DEF: MMU_enabled)
        ENDIF                       ; (:LNOT: :DEF: debug_build)

        ;
        ; Copy the read-write data block from ROM to RAM.
        ;

        IMPORT  |Image$$RO$$Limit|  ; End of ROM code (=start of ROM data)
        IMPORT  |Image$$RW$$Base|   ; Base of RAM to initialize
        IMPORT  |Image$$ZI$$Base|   ; Base and limit of area
        IMPORT  |Image$$ZI$$Limit|  ; to zero initialize

        ldr r0,=|Image$$RO$$Limit|  ; Get pointer to ROM data
        ldr r1,=|Image$$RW$$Base|   ; and RAM copy
        ldr r3,=|Image$$ZI$$Base|        
        ; Zero init base => top of initialized data
                        
        cmp r0,r1                   ; Check that they are different
        beq %F1
0                
        cmp r1,r3                   ; Copy init data
        ldrcc r2,[r0],#4
        strcc r2,[r1],#4
        bcc %B0
1                
        ldr r1,=|Image$$ZI$$Limit|  ; Top of zero init segment
        mov r2,#0
2                
        cmp r3,r1                   ; Zero init
        strcc r2,[r3],#4
        bcc %B2

program
        ;
        ; Call the actual C program.
        ; Should never return.
        ;
        IMPORT C_vMain
        b C_vMain                   ; C Entry


InitStacks
        ; Don't use DRAM,such as stmfd,ldmfd......
        ; SVCstack is initialized before
        ; Under toolkit ver 2.50, 'msr cpsr,r1' can be used instead of 'msr cpsr_cxsf,r1'
        
        mrs r0,cpsr
        bic r0,r0,#MODEMASK
        orr r1,r0,#UNDEFMODE|NOINT
        msr cpsr_cxsf,r1            ; UndefMode
        ldr sp,=UndefStack
        
        orr r1,r0,#ABORTMODE|NOINT
        msr cpsr_cxsf,r1            ; AbortMode
        ldr sp,=AbortStack

        orr r1,r0,#IRQMODE|NOINT
        msr cpsr_cxsf,r1            ; IRQMode
        ldr sp,=IRQStack
        
        orr r1,r0,#FIQMODE|NOINT
        msr cpsr_cxsf,r1            ; FIQMode
        ldr sp,=FIQStack

        bic r0,r0,#MODEMASK|NOINT
        orr r1,r0,#SVCMODE
        msr cpsr_cxsf,r1            ; SVCMode
        ldr sp,=SVCStack

        ; USER mode is not initialized.

        mov pc,lr                   ; The LR register may be not valid for the mode changes.

;
;*****************************************************************************
;
; Zero-initialized read/write data area for stacks.
; This area is determined by the RW value in the Linker under "entry and base".  
;*****************************************************************************
        AREA  SYS_STK, DATA, READWRITE, NOINIT

;*****************************************************************************
;
; Memory buffers to contain the stacks for the various processor modes which
; we will be using.
;
;*****************************************************************************

        ^       STK_SADDR

UserStack       #       _SVC_STKSIZE
SVCStack        #       _UND_STKSIZE
UndefStack      #       _ABT_STKSIZE
AbortStack      #       _IRQ_STKSIZE
IRQStack        #       _FIQ_STKSIZE
FIQStack        #       0 


        AREA  ISR_HOOK, DATA, READWRITE, NOINIT

        ^  ISR_BADDR
HandleReset     # 4
HandleUndef     # 4
HandleSWI       # 4
HandlePabort    # 4
HandleDabort    # 4
HandleReserved  # 4
HandleIRQ       # 4
HandleFIQ       # 4

;
;*****************************************************************************
;        
        END
