/**************************************************************************
*                                                                         *
*   PROJECT     : ARM port for UCOS-II                                    *
*                                                                         *
*   MODULE      : FRMWRK.c                                                *
*                                                                         *
*   AUTHOR      : Michael Anburaj                                         *
*                 URL  : http://geocities.com/michaelanburaj/             *
*                 EMAIL: michaelanburaj@hotmail.com                       *
*                                                                         *
*   PROCESSOR   : EP7312 (32 bit ARM720T RISC core from CIRRUS Logic)     *
*                                                                         *
*   IDE         : SDT 2.51 & ADS 1.2                                      *
*                                                                         *
*   DESCRIPTION :                                                         *
*   This is the Framework module. Creates an operating infrastructure.    *
*                                                                         *
**************************************************************************/


#include "includes.h"
#include "frmwrk.h"
#include "consol.h"

#include <stdarg.h>
#include <stdio.h>


/* ********************************************************************* */
/* Global definitions */


/* ********************************************************************* */
/* File local definitions */

#define __nSVCStkBase (STK_SADDR+_SVC_STKSIZE)
#define __nSVCStkEnd  STK_SADDR
#define __nSVCStkSize _SVC_STKSIZE

#define __nIRQStkBase (STK_SADDR+_SVC_STKSIZE+_UND_STKSIZE+_ABT_STKSIZE+_IRQ_STKSIZE)
#define __nIRQStkEnd  (STK_SADDR+_SVC_STKSIZE+_UND_STKSIZE+_ABT_STKSIZE)
#define __nIRQStkSize _IRQ_STKSIZE


/* ********************************************************************* */
/* Local functions */

/*
*********************************************************************************************
*                                       __Haltx
*
* Description: These exception handlers are used to report a particular exception on the
*              CONSOL & halt.
*
* Arguments  : none.
*
* Return     : none.
*
* Note(s)    : 
*********************************************************************************************
*/

static void __HaltUndef(void)
{
        CONSOL_Printf("Undefined instruction exception!!!\n");
        while(1);
}

static void __HaltSwi(void)
{
        CONSOL_Printf("SWI exception!!!\n");

        while(1);
}

static void __HaltPabort()
{
        CONSOL_Printf("Pabort exception!!!\n");
        while(1);
}

static void __HaltDabort(void)
{
        CONSOL_Printf("Dabort exception !!!\n");
        while(1);
}

static void __HaltFiq(void)
{
        CONSOL_Printf("FIQ exception!!!\n");
        while(1);
}

static void __HaltIrq(void)
{
        CONSOL_Printf("IRQ exception!!!\n");
        while(1);
}

/*
*********************************************************************************************
*                                       __vInitHaltHandlers
*
* Description: This routine hooks all the Halt exception handlers. Exceptions which are to
*              be handled by the application can be overwritten with the new handler at the
*              appropriate Software hook location.
*
* Arguments  : none.
*
* Return     : none.
*
* Note(s)    : 
*********************************************************************************************
*/

static void __vInitHaltHandlers(void)
{
        pISR_UNDEF = (unsigned)__HaltUndef;
        pISR_SWI   = (unsigned)__HaltSwi;
        pISR_PABORT= (unsigned)__HaltPabort;
        pISR_DABORT= (unsigned)__HaltDabort;
        pISR_FIQ   = (unsigned)__HaltFiq;
        pISR_IRQ = (unsigned)__HaltIrq;
}

/*
*********************************************************************************************
*                                       __vLogo
*
* Description: This routine display the Logo on the CONSOL port.
*
* Arguments  : none.
*
* Return     : none.
*
* Note(s)    : 
*********************************************************************************************
*/

void __vLogo(void)
{
        CONSOL_Printf("\n\nUCOS_FRAMEWORK Ver %s for %s :%s\n",
                      __mVer,__mProcessor,__mDate);
        CONSOL_Printf("Built using %s on %s <%s>\n",
#ifdef ADS
                      "ADS"
#elif SDT
                      "SDT"
#else
                      "Unknown IDE"
#endif
                      ,__DATE__,__TIME__);
        CONSOL_Printf("Endian: %s\n",
#ifdef __BIG_ENDIAN
                      "BIG"
#else
                      "LITTLE"
#endif
                      );
        CONSOL_Printf("CONSOL: COM%d, %dbps, 8Bit, NP\n",__nConsolPort,__nConsolBaud);
        CONSOL_Printf("CPU Clk: %dMHz   MMU: %s   Cache: %s   Write Buf: %s\n",MCLK/1000000,
#ifdef MMU_enabled
                       "ON","ON","ON");
#else
                       "OFF","OFF","OFF");
#endif
        CONSOL_Printf("FLASH_SADDR:%08xh SRAM_SADDR:%08xh SRAM_EADDR:%08xh\nSDRAM_SADDR:\
%08xh SFR_BADDR :%08xh ISR_BADDR :%08xh\n",FLASH_SADDR,SRAM_SADDR,SRAM_EADDR,SDRAM_SADDR,
SFR_BADDR,ISR_BADDR);
        CONSOL_Printf("Free RAM: %08xh ~ %08xh\n", C_wRTMem, C_wRTMemLimit);
        CONSOL_Printf("Developed by %s\n\n",__mAuthor);
}


/* ********************************************************************* */
/* Global functions */

/*
*********************************************************************************************
*                                       C_IRQHandler
*
* Description: This routine is the IRQ handler. Handles UART1 interrupts.
*
* Arguments  : none.
*
* Return     : none.
*
* Note(s)    : 
*********************************************************************************************
*/

void C_IRQHandler(void)
{
        U32 wTemp;

        /* Process INTSR1 interrupts */
        wTemp = rINTSR1&rINTMR1;

        if(TEST_BIT_SET(wTemp,BIT_EINT3)) /* High priority since its used for CS8900A */
        {
                ((void(*)(void))pISR_EINT3)();
        }

        if(TEST_BIT_SET(wTemp,BIT_TC1OI))
        {
                rTC1EOI = 0;
                ((void(*)(void))pISR_TC1OI)();
        }

        if(TEST_BIT_SET(wTemp,BIT_TC2OI))
        {
                rTC2EOI = 0;
                ((void(*)(void))pISR_TC2OI)();
        }

        if(TEST_BIT_SET(wTemp,BIT_URXINT1))
        {
                ((void(*)(void))pISR_URXINT1)();
        }

        if(TEST_BIT_SET(wTemp,BIT_UTXINT1))
        {
                ((void(*)(void))pISR_UTXINT1)();
        }

        /* Add code for the other IRQ Group 1 interrupts here */

        /* Process INTSR2 interrupts */
        wTemp = rINTSR2&rINTMR2;

        if(TEST_BIT_SET(wTemp,BIT_URXINT2))
        {
                ((void(*)(void))pISR_URXINT2)();
        }

        if(TEST_BIT_SET(wTemp,BIT_UTXINT2))
        {
                ((void(*)(void))pISR_UTXINT2)();
        }

        /* Add code for the other IRQ Group 2 interrupts here */
}

/*
*********************************************************************************************
*                                       FRMWRK_vStartTicker
*
* Description: This routine starts Timer1 (TC1) in the Prescale mode for OS Tick.
*
* Arguments  : wTicksPerSec - Time ticks per second.
*
* Return     : none.
*
* Note(s)    : 
*********************************************************************************************
*/

void FRMWRK_vStartTicker(U32 wTicksPerSec)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif

        OS_ENTER_CRITICAL();

        pISR_TC1OI = (U32)OSTimeTick;

//first, set up the timer1 for 512Khz input with prescale mode enabled
        BIT_SET(rSYSCON1,BIT_TC1M);        //set bit 4
        BIT_SET(rSYSCON1,BIT_TC1S);        //set bit 5

        rTC1D = (512000/wTicksPerSec-1);

        //enable timer interrupt by enabling the bit in the interrupt mask register
        rTC1EOI = 0;                       //clear the EOI
        BIT_SET(rINTMR1,BIT_TC1OI);        //set bit 8

        OS_EXIT_CRITICAL();
 }

/*
*********************************************************************************************
*                                       FRMWRK_vRamTest
*
* Description: This routine is used do RAM test.
*
* Arguments  : pbID   - Test identification string.
*              wSAddr - RAM starting address.
*              wEAddr - RAM end address.
*
* Return     : none.
*
* Note(s)    : Do not test the following areas using this routine:
*                       1. Stack
*                       2. Software vector table
*                       3. C Variables (RW & ZI)
*********************************************************************************************
*/

void FRMWRK_vRamTest(U8 *pbID,U32 wSAddr,U32 wEAddr)
{
        U32 wI,wJ;
        U8 bError=0;
        
        CONSOL_Printf("\n%s(%08xh-%08xh):WR",pbID,wSAddr,wEAddr);
        for(wI=wSAddr;wI<wEAddr;wI+=4)
        {
                *((volatile unsigned *)wI)=wI;
        }
        CONSOL_Printf("\b\bRD");
        for(wI=wSAddr;wI<wEAddr;wI+=4)
        {
                wJ=*((volatile unsigned *)wI);
                if(wJ!=wI)bError=1;
        }

        if(bError==0)CONSOL_Printf("\b\bO.K.\n");
        else CONSOL_Printf("\b\bFAIL\n");
}

/*
*********************************************************************************************
*                                       C_vMain
*
* Description: This is the main C entry function.
*
* Arguments  : none.
*
* Return     : none.
*
* Note(s)    : 
*********************************************************************************************
*/

void C_vMain(void)
{
        __vInitHaltHandlers();

        pISR_IRQ = (U32)UCOS_IRQHandler;

        CONSOL_Select(__nConsolPort);
        CONSOL_Init(__nConsolBaud,__nConsolFifoEn);

        __vLogo();
        
        APP_vMain();

        CONSOL_Printf("APP has ended...\n");
}

/*
*********************************************************************************************
*                                       printf
*
* Description: This is a stdio.h function.
*
* Arguments  : .
*
* Return     : Returns 0.
*
* Note(s)    : 
*********************************************************************************************
*/

int printf(const char *pbFmt,...)
{
        va_list pArg;
        char abString[1024];

        va_start(pArg,pbFmt);
        vsprintf(abString,pbFmt,pArg);
        CONSOL_SendString(abString);
        va_end(pArg);

        return 0;
}


/* ********************************************************************* */
