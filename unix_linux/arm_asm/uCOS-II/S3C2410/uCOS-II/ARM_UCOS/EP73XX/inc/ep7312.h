/**************************************************************************
*                                                                         *
*   PROJECT     : ARM port for UCOS-II                                    *
*                                                                         *
*   MODULE      : EP7312.h                                                *
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
*   EP7312 processor register definition header file.                     *
*                                                                         *
**************************************************************************/


#ifndef __EP7312_H__
#define __EP7312_H__


#include "frmwrk.h"


#ifdef __cplusplus
extern "C" {
#endif


/* ********************************************************************* */
/* Module configuration */


/* ********************************************************************* */
/* Interface macro & data definition */

/* ID */
#define rUNIQID         (*(volatile U32 *)(SFR_BADDR + 0x2440))
#define rRANDID0        (*(volatile U32 *)(SFR_BADDR + 0x2700))
#define rRANDID1        (*(volatile U32 *)(SFR_BADDR + 0x2704))
#define rRANDID2        (*(volatile U32 *)(SFR_BADDR + 0x2708))
#define rRANDID3        (*(volatile U32 *)(SFR_BADDR + 0x270c))

/* System */
#define rSYSCON1        (*(volatile U32 *)(SFR_BADDR + 0x0100))
#define rSYSFLG1        (*(volatile U32 *)(SFR_BADDR + 0x0140))
#define rSYSCON2        (*(volatile U32 *)(SFR_BADDR + 0x1100))
#define rSYSFLG2        (*(volatile U32 *)(SFR_BADDR + 0x1140))
#define rSYSCON3        (*(volatile U32 *)(SFR_BADDR + 0x2200))
#define rSTFCLR         (*(volatile U32 *)(SFR_BADDR + 0x05c0))
#define rHALT           (*(volatile U32 *)(SFR_BADDR + 0x0800))
#define rSTDBY          (*(volatile U32 *)(SFR_BADDR + 0x0840))
#define rPLLW           (*(volatile U32 *)(SFR_BADDR + 0x2610))
#define rPLLR           (*(volatile U32 *)(SFR_BADDR + 0xa5a8))

/* Memory */
#define rMEMCFG1        (*(volatile U32 *)(SFR_BADDR + 0x0180))
#define rMEMCFG2        (*(volatile U32 *)(SFR_BADDR + 0x01c0))
#define rSDCONF         (*(volatile U32 *)(SFR_BADDR + 0x2300))
#define rSDRFPR         (*(volatile U32 *)(SFR_BADDR + 0x2340))

/* Interrupt */
#define rINTSR1         (*(volatile U32 *)(SFR_BADDR + 0x0240))
#define rINTMR1         (*(volatile U32 *)(SFR_BADDR + 0x0280))
#define rINTSR2         (*(volatile U32 *)(SFR_BADDR + 0x1240))
#define rINTMR2         (*(volatile U32 *)(SFR_BADDR + 0x1280))
#define rINTSR3         (*(volatile U32 *)(SFR_BADDR + 0x2240))
#define rINTMR3         (*(volatile U32 *)(SFR_BADDR + 0x2280))

/* EOI */
#define rBLEOI          (*(volatile U32 *)(SFR_BADDR + 0x0600))
#define rMCEOI          (*(volatile U32 *)(SFR_BADDR + 0x0640))
#define rTEOI           (*(volatile U32 *)(SFR_BADDR + 0x0680))
#define rTC1EOI         (*(volatile U32 *)(SFR_BADDR + 0x06c0))
#define rTC2EOI         (*(volatile U32 *)(SFR_BADDR + 0x0700))
#define rRTCEOI         (*(volatile U32 *)(SFR_BADDR + 0x0740))
#define rUMSEOI         (*(volatile U32 *)(SFR_BADDR + 0x0780))
#define rCOEOI          (*(volatile U32 *)(SFR_BADDR + 0x07c0))
#define rKBDEOI         (*(volatile U32 *)(SFR_BADDR + 0x1700))

/* GPIO */
#ifdef __BIG_ENDIAN
#define rPADR           (*(volatile U8 *)(SFR_BADDR + 0x0003))
#define rPADDR          (*(volatile U8 *)(SFR_BADDR + 0x0043))
#define rPBDR           (*(volatile U8 *)(SFR_BADDR + 0x0002))
#define rPBDDR          (*(volatile U8 *)(SFR_BADDR + 0x0042))
#define rPDDR           (*(volatile U8 *)(SFR_BADDR + 0x0000))
#define rPDDDR          (*(volatile U8 *)(SFR_BADDR + 0x0040))
#define rPEDR           (*(volatile U8 *)(SFR_BADDR + 0x0083))
#define rPEDDR          (*(volatile U8 *)(SFR_BADDR + 0x00c3))
#else
#define rPADR           (*(volatile U8 *)(SFR_BADDR + 0x0000))
#define rPADDR          (*(volatile U8 *)(SFR_BADDR + 0x0040))
#define rPBDR           (*(volatile U8 *)(SFR_BADDR + 0x0001))
#define rPBDDR          (*(volatile U8 *)(SFR_BADDR + 0x0041))
#define rPDDR           (*(volatile U8 *)(SFR_BADDR + 0x0003))
#define rPDDDR          (*(volatile U8 *)(SFR_BADDR + 0x0043))
#define rPEDR           (*(volatile U8 *)(SFR_BADDR + 0x0080))
#define rPEDDR          (*(volatile U8 *)(SFR_BADDR + 0x00c0))
#endif

/* UART */
#define rUARTDR1        (*(volatile U32 *)(SFR_BADDR + 0x0480))
#define rUBRLCR1        (*(volatile U32 *)(SFR_BADDR + 0x04c0))
#define rUARTDR2        (*(volatile U32 *)(SFR_BADDR + 0x1480))
#define rUBRLCR2        (*(volatile U32 *)(SFR_BADDR + 0x14c0))

/* SSI */
#define rSYNCIO         (*(volatile U32 *)(SFR_BADDR + 0x0500))
#define rSS2DR          (*(volatile U32 *)(SFR_BADDR + 0x1500))
#define rSRXEOF         (*(volatile U32 *)(SFR_BADDR + 0x1600))
#define rSS2POP         (*(volatile U32 *)(SFR_BADDR + 0x16c0))

/* Timer */
#define rTC1D           (*(volatile U32 *)(SFR_BADDR + 0x0300))
#define rTC2D           (*(volatile U32 *)(SFR_BADDR + 0x0340))

/* LCD */
#define rLCDCON         (*(volatile U32 *)(SFR_BADDR + 0x02c0))
#define rPALLSW         (*(volatile U32 *)(SFR_BADDR + 0x0540))
#define rPALMSW         (*(volatile U32 *)(SFR_BADDR + 0x0580))
#define rFBADDR         (*(volatile U32 *)(SFR_BADDR + 0x1000))

/* RTC */
#define rRTCDR          (*(volatile U32 *)(SFR_BADDR + 0x0380))
#define rRTCMR          (*(volatile U32 *)(SFR_BADDR + 0x03c0))

/* DAI */
#define rDAIR           (*(volatile U32 *)(SFR_BADDR + 0x2000))
#define rDAIR0          (*(volatile U32 *)(SFR_BADDR + 0x2040))
#define rDAIDR1         (*(volatile U32 *)(SFR_BADDR + 0x2080))
#define rDAIDR2         (*(volatile U32 *)(SFR_BADDR + 0x20c0))
#define rDAISR          (*(volatile U32 *)(SFR_BADDR + 0x2100))
#define rDAI64Fs        (*(volatile U32 *)(SFR_BADDR + 0x2600))

/* others */
#define rPMPCON         (*(volatile U32 *)(SFR_BADDR + 0x0400))
#define rCODR           (*(volatile U32 *)(SFR_BADDR + 0x0440))
#define rLEDFLSH        (*(volatile U32 *)(SFR_BADDR + 0x22c0))

/* NAND Flash */
#define rEE_SADDR       (*(volatile U32 *)(EE_SADDR + 0x0000))

/* CDK238 Specific registers */
/* Ethernet Controller, CS8900A */
#define rETHRxTx0       (*(volatile U16 *)(ETH_BADDR + ETH_IOBADDR + 0x0000))
#define rETHRxTx1       (*(volatile U16 *)(ETH_BADDR + ETH_IOBADDR + 0x0002))
#define rETHTxCMD       (*(volatile U16 *)(ETH_BADDR + ETH_IOBADDR + 0x0004))
#define rETHTxLEN       (*(volatile U16 *)(ETH_BADDR + ETH_IOBADDR + 0x0006))
#define rETHINTSTATQ    (*(volatile U16 *)(ETH_BADDR + ETH_IOBADDR + 0x0008))
#define rETHPKTPGPTR    (*(volatile U16 *)(ETH_BADDR + ETH_IOBADDR + 0x000a))
#define rETHPKTPGDATA0  (*(volatile U16 *)(ETH_BADDR + ETH_IOBADDR + 0x000c))
#define rETHPKTPGDATA1  (*(volatile U16 *)(ETH_BADDR + ETH_IOBADDR + 0x000e))


/* Soft Vector Locations */
/* Exceptions */
#define pISR_RESET      (*(U32 *)(ISR_BADDR+0x00))
#define pISR_UNDEF      (*(U32 *)(ISR_BADDR+0x04))
#define pISR_SWI        (*(U32 *)(ISR_BADDR+0x08))
#define pISR_PABORT     (*(U32 *)(ISR_BADDR+0x0c))
#define pISR_DABORT     (*(U32 *)(ISR_BADDR+0x10))
#define pISR_RESERVED   (*(U32 *)(ISR_BADDR+0x14))
#define pISR_IRQ        (*(U32 *)(ISR_BADDR+0x18))
#define pISR_FIQ        (*(U32 *)(ISR_BADDR+0x1c))

/* HW interrupts */
#define pISR_EXTFIQ     (*(U32 *)(ISR_BADDR+0x20))
#define pISR_BLINT      (*(U32 *)(ISR_BADDR+0x24))
#define pISR_WEINT      (*(U32 *)(ISR_BADDR+0x28))
#define pISR_MCINT      (*(U32 *)(ISR_BADDR+0x2c))
#define pISR_CSINT      (*(U32 *)(ISR_BADDR+0x30))
#define pISR_EINT1      (*(U32 *)(ISR_BADDR+0x34))
#define pISR_EINT2      (*(U32 *)(ISR_BADDR+0x38))
#define pISR_EINT3      (*(U32 *)(ISR_BADDR+0x3c))
#define pISR_TC1OI      (*(U32 *)(ISR_BADDR+0x40))
#define pISR_TC2OI      (*(U32 *)(ISR_BADDR+0x44))
#define pISR_RTCMI      (*(U32 *)(ISR_BADDR+0x48))
#define pISR_TINT       (*(U32 *)(ISR_BADDR+0x4c))
#define pISR_UTXINT1    (*(U32 *)(ISR_BADDR+0x50))
#define pISR_URXINT1    (*(U32 *)(ISR_BADDR+0x54))
#define pISR_UMSINT     (*(U32 *)(ISR_BADDR+0x58))
#define pISR_SSEOTI     (*(U32 *)(ISR_BADDR+0x5c))
#define pISR_KBDINT     (*(U32 *)(ISR_BADDR+0x60))
#define pISR_SS2RX      (*(U32 *)(ISR_BADDR+0x64))
#define pISR_SS2TX      (*(U32 *)(ISR_BADDR+0x68))
#define pISR_UTXINT2    (*(U32 *)(ISR_BADDR+0x6c))
#define pISR_URXINT2    (*(U32 *)(ISR_BADDR+0x70))
#define pISR_DAIINT     (*(U32 *)(ISR_BADDR+0x74))


/* Bit Definitions */
/* INTSR1/INTMR1 */
#define BIT_EXTFIQ      (1<<0)
#define BIT_BLINT       (1<<1)
#define BIT_WEINT       (1<<2)
#define BIT_MCINT       (1<<3)
#define BIT_CSINT       (1<<4)
#define BIT_EINT1       (1<<5)
#define BIT_EINT2       (1<<6)
#define BIT_EINT3       (1<<7)
#define BIT_TC1OI       (1<<8)
#define BIT_TC2OI       (1<<9)
#define BIT_RTCMI       (1<<10)
#define BIT_TINT        (1<<11)
#define BIT_UTXINT1     (1<<12)
#define BIT_URXINT1     (1<<13)
#define BIT_UMSINT      (1<<14)
#define BIT_SSEOTI      (1<<15)

/* INTSR2/INTMR2 */
#define BIT_KBDINT      (1<<0)
#define BIT_SS2RX       (1<<1)
#define BIT_SS2TX       (1<<2)
#define BIT_UTXINT2     (1<<12)
#define BIT_URXINT2     (1<<13)

/* INTSR3/INTMR3 */
#define BIT_DAIINT      (1<<0)

/* SYSCON1 */
#define BIT_TC1M        (1<<4)
#define BIT_TC1S        (1<<5)
#define BIT_TC2M        (1<<6)
#define BIT_TC2S        (1<<7)
#define BIT_UART1EN     (1<<8)
#define BIT_BZTOG       (1<<9)
#define BIT_BZMOD       (1<<10)
#define BIT_DBGEN       (1<<11)
#define BIT_LCDEN       (1<<12)
#define BIT_CDENTX      (1<<13)
#define BIT_CDENRX      (1<<14)
#define BIT_SIREN       (1<<15)
#define BIT_EXCKEN      (1<<18)
#define BIT_WAKEDIS     (1<<19)
#define BIT_IRTXM       (1<<20)

/* SYSCON2 */
#define BIT_UART2EN     (1<<8)

/* LCDCON */
#define BIT_GSEN        (1<<30)
#define BIT_GSMD        (1<<31)

/* ARM modes */
#define USERMODE        0x10
#define FIQMODE         0x11
#define IRQMODE         0x12
#define SVCMODE         0x13
#define ABORTMODE       0x17
#define UNDEFMODE       0x1b
#define MODEMASK        0x1f
#define NOINT           0xc0


/* ********************************************************************* */
/* Interface function definition */


/* ********************************************************************* */

#ifdef __cplusplus
}
#endif

#endif /*__EP7312_H__*/
