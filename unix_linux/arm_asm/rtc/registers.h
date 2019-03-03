/*******************************************************************************
* File: registers.h
* For: S3C6410
* 
* Description:
*   Define all the registers address.
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-29
*******************************************************************************/

#ifndef REGISTERS_H
#define REGISTERS_H


/*******************************************************************************
* 3 - SYSTEM CONTROLLER
*******************************************************************************/

/* PLL Control Registers */
#define         APLL_LOCK           (*(volatile unsigned int *) (0x7E00F000))
#define         MPLL_LOCK           (*(volatile unsigned int *) (0x7E00F004))
#define         EPLL_LOCK           (*(volatile unsigned int *) (0x7E00F008))

#define         APLL_CON            (*(volatile unsigned int *) (0x7E00F00C))
#define         MPLL_CON            (*(volatile unsigned int *) (0x7E00F010))
#define         EPLL_CON0           (*(volatile unsigned int *) (0x7E00F014))
#define         EPLL_CON1           (*(volatile unsigned int *) (0x7E00F018))

/* Clock Source Control Registers */
#define         CLK_SRC             (*(volatile unsigned int *) (0x7E00F01C))
#define         CLK_SRC2            (*(volatile unsigned int *) (0x7E00F10C))

/* Clock Divider Control Registers */
#define         CLK_DIV0            (*(volatile unsigned int *) (0x7E00F020))
#define         CLK_DIV1            (*(volatile unsigned int *) (0x7E00F024))
#define         CLK_DIV2            (*(volatile unsigned int *) (0x7E00F028))

/* Clock Output Configuration Register */
#define         CLK_OUT             (*(volatile unsigned int *) (0x7E00F02C))

/* Clock Gating Control Registers */
#define         HCLK_GATE           (*(volatile unsigned int *) (0x7E00F030))
#define         PCLK_GATE           (*(volatile unsigned int *) (0x7E00F034))
#define         SCLK_GATE           (*(volatile unsigned int *) (0x7E00F038))
#define         MEM0_CLK_GATE       (*(volatile unsigned int *) (0x7E00F03C))

/* AHB Bus Control Registers */
#define         AHB_CON0            (*(volatile unsigned int *) (0x7E00F100))
#define         AHB_CON1            (*(volatile unsigned int *) (0x7E00F104))
#define         AHB_CON2            (*(volatile unsigned int *) (0x7E00F108))

/* Secure DMA Control Register */
#define         SDMA_SEL            (*(volatile unsigned int *) (0x7E00F110))

/* System ID Register */
#define         SYS_ID              (*(volatile unsigned int *) (0x7E00F118))

/* System Others Register */
#define         SYS_OTHERS          (*(volatile unsigned int *) (0x7E00F11C))

/* Memory Controller Status Registers */
#define         MEM_SYS_CFG         (*(volatile unsigned int *) (0x7E00F120))
#define         QOS_OVERRIDE1       (*(volatile unsigned int *) (0x7E00F128))
#define         MEM_CFG_STAT        (*(volatile unsigned int *) (0x7E00F12C))

/* Power Mode Control Registers */
#define         PWR_CFG             (*(volatile unsigned int *) (0x7E00F804))
#define         EINT_MASK           (*(volatile unsigned int *) (0x7E00F808))
#define         NORMAL_CFG          (*(volatile unsigned int *) (0x7E00F810))
#define         STOP_CFG            (*(volatile unsigned int *) (0x7E00F814))
#define         SLEEP_CFG           (*(volatile unsigned int *) (0x7E00F818))
#define         STOP_MEM_CFG        (*(volatile unsigned int *) (0x7E00F81C))

/* System Stabilization Counter */
#define         OSC_FREQ            (*(volatile unsigned int *) (0x7E00F820))
#define         OSC_STABLE          (*(volatile unsigned int *) (0x7E00F824))
#define         PWR_STABLE          (*(volatile unsigned int *) (0x7E00F828))
#define         MTC_STABLE          (*(volatile unsigned int *) (0x7E00F830))

/* Cacheable Bus Transaction Selection Register */
#define         MISC_CON            (*(volatile unsigned int *) (0x7E00F838))

/* Other Control Register */
#define         OTHERS              (*(volatile unsigned int *) (0x7E00F900))

/* Status Registers */
#define         RST_STAT            (*(volatile unsigned int *) (0x7E00F904))
#define         WAKEUP_STAT         (*(volatile unsigned int *) (0x7E00F908))
#define         BLK_PWR_STAT        (*(volatile unsigned int *) (0x7E00F90C))

/* Information Registers */
#define         INFORM0             (*(volatile unsigned int *) (0x7E00FA00))
#define         INFORM1             (*(volatile unsigned int *) (0x7E00FA04))
#define         INFORM2             (*(volatile unsigned int *) (0x7E00FA08))
#define         INFORM3             (*(volatile unsigned int *) (0x7E00FA0C))

/*******************************************************************************
* 5 - DRAM CONTROLLER
*******************************************************************************/

/* DRAM Controller Status Register */
#define         P1MEMSTAT           (*(volatile unsigned int *) (0x7E001000))

/* DRAM Controller Command Register */
#define         P1MEMCCMD           (*(volatile unsigned int *) (0x7E001004))

/* Direct Command Register */
#define         P1DIRECTCMD         (*(volatile unsigned int *) (0x7E001008))

/* Memory Configuration 1 Register */
#define         P1MEMCFG            (*(volatile unsigned int *) (0x7E00100C))

/* Refresh Period Register */
#define         P1REFRESH           (*(volatile unsigned int *) (0x7E001010))

/* CAS Latency Register */
#define         P1CASLAT            (*(volatile unsigned int *) (0x7E001014))

/* T_DQSS Register */
#define         P1T_DQSS            (*(volatile unsigned int *) (0x7E001018))

/* T_MRD Register */
#define         P1T_MRD             (*(volatile unsigned int *) (0x7E00101C))

/* T_RAS Register */
#define         P1T_RAS             (*(volatile unsigned int *) (0x7E001020))

/* T_RC Register */
#define         P1T_RC              (*(volatile unsigned int *) (0x7E001024))

/* T_RCD Register */
#define         P1T_RCD             (*(volatile unsigned int *) (0x7E001028))

/* T_RFC Register */
#define         P1T_RFC             (*(volatile unsigned int *) (0x7E00102C))

/* T_RP Register */
#define         P1T_RP              (*(volatile unsigned int *) (0x7E001030))

/* T_RRD Register */
#define         P1T_RRD             (*(volatile unsigned int *) (0x7E001034))

/* T_WR Register */
#define         P1T_WR              (*(volatile unsigned int *) (0x7E001038))

/* T_WTR Register */
#define         P1T_WTR             (*(volatile unsigned int *) (0x7E00103C))

/* T_XP Register */
#define         P1T_XP              (*(volatile unsigned int *) (0x7E001040))

/* T_XSR Register */
#define         P1T_XSR             (*(volatile unsigned int *) (0x7E001044))

/* T_ESR Register */
#define         P1T_ESR             (*(volatile unsigned int *) (0x7E001048))

/* Memory Configuration 2 Register */
#define         P1MEMCFG2           (*(volatile unsigned int *) (0x7E00104C))

/* Memory Configuration 3 Register */
#define         P1MEMCFG3           (*(volatile unsigned int *) (0x7E001050))

/* ID_N_CFG Registers */
#define         P1ID0CFG            (*(volatile unsigned int *) (0x7E001100))

/* CHIP_N_CFG Registers */
#define         P1CHIP0CFG          (*(volatile unsigned int *) (0x7E001200))
#define         P1CHIP1CFG          (*(volatile unsigned int *) (0x7E001204))

/* USER_STATUS Register */
#define         P1USERSTAT          (*(volatile unsigned int *) (0x7E001300))

/* USER_CONFIG Register */
#define         P1USERCFG           (*(volatile unsigned int *) (0x7E001304))

/*******************************************************************************
* 8 - NAND FLASH CONTROLLER
*******************************************************************************/

/* NAND Flash Configuration Register */
#define         NFCONF              (*(volatile unsigned int *) (0x70200000))

/* Control Register */
#define         NFCONT              (*(volatile unsigned int *) (0x70200004))

/* Command Register */
#define         NFCMMD              (*(volatile unsigned int *) (0x70200008))

/* Address Register */
#define         NFADDR              (*(volatile unsigned int *) (0x7020000C))

/* Data Register */
#define         NFDATA              (*(volatile unsigned int *) (0x70200010))

/* Main Data Area ECC Register */
#define         NFMECCD0            (*(volatile unsigned int *) (0x70200014))
#define         NFMECCD1            (*(volatile unsigned int *) (0x70200018))

/* Spare Area ECC Register */
#define         NFSECCD             (*(volatile unsigned int *) (0x7020001C))

/* Programmable Block Address Register */
#define         NFSBLK              (*(volatile unsigned int *) (0x70200020))
#define         NFEBLK              (*(volatile unsigned int *) (0x70200024))

/* NFCON Status Register */
#define         NFSTAT              (*(volatile unsigned int *) (0x70200028))

/* ECC0/1 Error Status Register */
#define         NFECCERR0           (*(volatile unsigned int *) (0x7020002C))
#define         NFECCERR1           (*(volatile unsigned int *) (0x70200030))

/* Main Data Area ECC0 Status Registers */
#define         NFMECC0             (*(volatile unsigned int *) (0x70200034))
#define         NFMECC1             (*(volatile unsigned int *) (0x70200038))

/* Spare Area ECC Status Register */
#define         NFSECC              (*(volatile unsigned int *) (0x7020003C))

/* 4-BIT ECC ERROR Patten Register */
#define         NFMLCBITPT          (*(volatile unsigned int *) (0x70200040))

/* ECC 0/1/2 For 8-BIT ECC Status Registers */
#define         NF8ECCERR0          (*(volatile unsigned int *) (0x70200044))
#define         NF8ECCERR1          (*(volatile unsigned int *) (0x70200048))
#define         NF8ECCERR2          (*(volatile unsigned int *) (0x7020004C))

/* 8-BIT ECC Main Data ECC 0/1/2/3 Status Registers */
#define         NFM8ECC0            (*(volatile unsigned int *) (0x70200050))
#define         NFM8ECC1            (*(volatile unsigned int *) (0x70200054))
#define         NFM8ECC2            (*(volatile unsigned int *) (0x70200058))
#define         NFM8ECC3            (*(volatile unsigned int *) (0x7020005C))

/* 8-BIT ECC Error Pattern Register */
#define         NFMLC8BITPT0        (*(volatile unsigned int *) (0x70200060))
#define         NFMLC8BITPT1        (*(volatile unsigned int *) (0x70200064))

/*******************************************************************************
* 10 - GPIO
*******************************************************************************/

/* PORT A CONTROL REGISTERS */
#define         GPACON              (*(volatile unsigned int *) (0x7F008000))
#define         GPADAT              (*(volatile unsigned int *) (0x7F008004))
#define         GPAPUD              (*(volatile unsigned int *) (0x7F008008))
#define         GPACONSLP           (*(volatile unsigned int *) (0x7F00800C))
#define         GPAPUDSLP           (*(volatile unsigned int *) (0x7F008010))

/* PORT B CONTROL REGISTERS */
#define         GPBCON              (*(volatile unsigned int *) (0x7F008020))
#define         GPBDAT              (*(volatile unsigned int *) (0x7F008024))
#define         GPBPUD              (*(volatile unsigned int *) (0x7F008028))
#define         GPBCONSLP           (*(volatile unsigned int *) (0x7F00802C))
#define         GPBPUDSLP           (*(volatile unsigned int *) (0x7F008030))

/* PORT C CONTROL REGISTERS */
#define         GPCCON              (*(volatile unsigned int *) (0x7F008040))
#define         GPCDAT              (*(volatile unsigned int *) (0x7F008044))
#define         GPCPUD              (*(volatile unsigned int *) (0x7F008048))
#define         GPCCONSLP           (*(volatile unsigned int *) (0x7F00804C))
#define         GPCPUDSLP           (*(volatile unsigned int *) (0x7F008050))

/* PORT D CONTROL REGISTERS */
#define         GPDCON              (*(volatile unsigned int *) (0x7F008060))
#define         GPDDAT              (*(volatile unsigned int *) (0x7F008064))
#define         GPDPUD              (*(volatile unsigned int *) (0x7F008068))
#define         GPDCONSLP           (*(volatile unsigned int *) (0x7F00806C))
#define         GPDPUDSLP           (*(volatile unsigned int *) (0x7F008070))

/* PORT E CONTROL REGISTERS */
#define         GPECON              (*(volatile unsigned int *) (0x7F008080))
#define         GPEDAT              (*(volatile unsigned int *) (0x7F008084))
#define         GPEPUD              (*(volatile unsigned int *) (0x7F008088))
#define         GPECONSLP           (*(volatile unsigned int *) (0x7F00808C))
#define         GPEPUDSLP           (*(volatile unsigned int *) (0x7F008090))

/* PORT F CONTROL REGISTERS */
#define         GPFCON              (*(volatile unsigned int *) (0x7F0080A0))
#define         GPFDAT              (*(volatile unsigned int *) (0x7F0080A4))
#define         GPFPUD              (*(volatile unsigned int *) (0x7F0080A8))
#define         GPFCONSLP           (*(volatile unsigned int *) (0x7F0080AC))
#define         GPFPUDSLP           (*(volatile unsigned int *) (0x7F0080B0))

/* PORT G CONTROL REGISTERS */
#define         GPGCON              (*(volatile unsigned int *) (0x7F0080C0))
#define         GPGDAT              (*(volatile unsigned int *) (0x7F0080C4))
#define         GPGPUD              (*(volatile unsigned int *) (0x7F0080C8))
#define         GPGCONSLP           (*(volatile unsigned int *) (0x7F0080CC))
#define         GPGPUDSLP           (*(volatile unsigned int *) (0x7F0080D0))

/* PORT H CONTROL REGISTERS */
#define         GPHCON0             (*(volatile unsigned int *) (0x7F0080E0))
#define         GPHCON1             (*(volatile unsigned int *) (0x7F0080E4))
#define         GPHDAT              (*(volatile unsigned int *) (0x7F0080E8))
#define         GPHPUD              (*(volatile unsigned int *) (0x7F0080EC))
#define         GPHCONSLP           (*(volatile unsigned int *) (0x7F0080F0))
#define         GPHPUDSLP           (*(volatile unsigned int *) (0x7F0080F4))

/* PORT I CONTROL REGISTERS */
#define         GPICON              (*(volatile unsigned int *) (0x7F008100))
#define         GPIDAT              (*(volatile unsigned int *) (0x7F008104))
#define         GPIPUD              (*(volatile unsigned int *) (0x7F008108))
#define         GPICONSLP           (*(volatile unsigned int *) (0x7F00810C))
#define         GPIPUDSLP           (*(volatile unsigned int *) (0x7F008110))

/* PORT J CONTROL REGISTERS */
#define         GPJCON              (*(volatile unsigned int *) (0x7F008120))
#define         GPJDAT              (*(volatile unsigned int *) (0x7F008124))
#define         GPJPUD              (*(volatile unsigned int *) (0x7F008128))
#define         GPJCONSLP           (*(volatile unsigned int *) (0x7F00812C))
#define         GPJPUDSLP           (*(volatile unsigned int *) (0x7F008130))

/* PORT K CONTROL REGISTERS */
#define         GPKCON0             (*(volatile unsigned int *) (0x7F008800))
#define         GPKCON1             (*(volatile unsigned int *) (0x7F008804))
#define         GPKDAT              (*(volatile unsigned int *) (0x7F008808))
#define         GPKPUD              (*(volatile unsigned int *) (0x7F00880C))

/* PORT L CONTROL REGISTERS */
#define         GPLCON0             (*(volatile unsigned int *) (0x7F008810))
#define         GPLCON1             (*(volatile unsigned int *) (0x7F008814))
#define         GPLDAT              (*(volatile unsigned int *) (0x7F008818))
#define         GPLPUD              (*(volatile unsigned int *) (0x7F00881C))

/* PORT M CONTROL REGISTERS */
#define         GPMCON              (*(volatile unsigned int *) (0x7F008820))
#define         GPMDAT              (*(volatile unsigned int *) (0x7F008824))
#define         GPMPUD              (*(volatile unsigned int *) (0x7F008828))

/* PORT N CONTROL REGISTERS */
#define         GPNCON              (*(volatile unsigned int *) (0x7F008830))
#define         GPNDAT              (*(volatile unsigned int *) (0x7F008834))
#define         GPNPUD              (*(volatile unsigned int *) (0x7F008838))

/* PORT O CONTROL REGISTERS */
#define         GPOCON              (*(volatile unsigned int *) (0x7F008140))
#define         GPODAT              (*(volatile unsigned int *) (0x7F008144))
#define         GPOPUD              (*(volatile unsigned int *) (0x7F008148))
#define         GPOCONSLP           (*(volatile unsigned int *) (0x7F00814C))
#define         GPOPUDSLP           (*(volatile unsigned int *) (0x7F008150))

/* PORT P CONTROL REGISTERS */
#define         GPPCON              (*(volatile unsigned int *) (0x7F008160))
#define         GPPDAT              (*(volatile unsigned int *) (0x7F008164))
#define         GPPPUD              (*(volatile unsigned int *) (0x7F008168))
#define         GPPCONSLP           (*(volatile unsigned int *) (0x7F00816C))
#define         GPPPUDSLP           (*(volatile unsigned int *) (0x7F008170))

/* PORT Q CONTROL REGISTERS */
#define         GPQCON              (*(volatile unsigned int *) (0x7F008180))
#define         GPQDAT              (*(volatile unsigned int *) (0x7F008184))
#define         GPQPUD              (*(volatile unsigned int *) (0x7F008188))
#define         GPQCONSLP           (*(volatile unsigned int *) (0x7F00818C))
#define         GPQPUDSLP           (*(volatile unsigned int *) (0x7F008190))

/* SPECIAL PORT CONTROL REGISTER */
#define         SPCON               (*(volatile unsigned int *) (0x7F0081A0))

/* MEMORY INTERFACE PIN CONFIGURATION REGISTER IN STOP MODE */
#define         MEM0CONSTOP         (*(volatile unsigned int *) (0x7F0081B0))
#define         MEM1CONSTOP         (*(volatile unsigned int *) (0x7F0081B4))

/* MEMORY INTERFACE PIN CONFIGURATION REGISTER IN SLEEP MODE */
#define         MEM0CONSLP0         (*(volatile unsigned int *) (0x7F0081C0))
#define         MEM0CONSLP1         (*(volatile unsigned int *) (0x7F0081C4))
#define         MEM1CONSLP          (*(volatile unsigned int *) (0x7F0081C8))

/* MEMORY INTERFACE DRIVE STRENGTH CONTROL REGISTER */
#define         MEM0DRVCON          (*(volatile unsigned int *) (0x7F0081D0))
#define         MEM1DRVCON          (*(volatile unsigned int *) (0x7F0081D4))

/* EXTERNAL INTERRUPT CONTROL REGISTERS */
#define         EINT0CON0           (*(volatile unsigned int *) (0x7F008900))
#define         EINT0CON1           (*(volatile unsigned int *) (0x7F008904))
#define         EINT0FLTCON0        (*(volatile unsigned int *) (0x7F008910))
#define         EINT0FLTCON1        (*(volatile unsigned int *) (0x7F008914))
#define         EINT0FLTCON2        (*(volatile unsigned int *) (0x7F008918))
#define         EINT0FLTCON3        (*(volatile unsigned int *) (0x7F00891C))
#define         EINT0MASK           (*(volatile unsigned int *) (0x7F008920))
#define         EINT0PEND           (*(volatile unsigned int *) (0x7F008924))
#define         EINT12CON           (*(volatile unsigned int *) (0x7F008200))
#define         EINT34CON           (*(volatile unsigned int *) (0x7F008204))
#define         EINT56CON           (*(volatile unsigned int *) (0x7F008208))
#define         EINT78CON           (*(volatile unsigned int *) (0x7F00820C))
#define         EINT9CON            (*(volatile unsigned int *) (0x7F008210))
#define         EINT12FLTCON        (*(volatile unsigned int *) (0x7F008220))
#define         EINT34FLTCON        (*(volatile unsigned int *) (0x7F008224))
#define         EINT56FLTCON        (*(volatile unsigned int *) (0x7F008228))
#define         EINT78FLTCON        (*(volatile unsigned int *) (0x7F00822C))
#define         EINT9FLTCON         (*(volatile unsigned int *) (0x7F008230))
#define         EINT12MASK          (*(volatile unsigned int *) (0x7F008240))
#define         EINT34MASK          (*(volatile unsigned int *) (0x7F008244))
#define         EINT56MASK          (*(volatile unsigned int *) (0x7F008248))
#define         EINT78MASK          (*(volatile unsigned int *) (0x7F00824C))
#define         EINT9MASK           (*(volatile unsigned int *) (0x7F008250))
#define         EINT12PEND          (*(volatile unsigned int *) (0x7F008260))
#define         EINT34PEND          (*(volatile unsigned int *) (0x7F008264))
#define         EINT56PEND          (*(volatile unsigned int *) (0x7F008268))
#define         EINT78PEND          (*(volatile unsigned int *) (0x7F00826C))
#define         EINT9PEND           (*(volatile unsigned int *) (0x7F008270))
#define         PRIORITY            (*(volatile unsigned int *) (0x7F008280))

/* CURRENT SERVICE REGIDTER(SERVICE) */
#define         SERVICE             (*(volatile unsigned int *) (0x7F008284))
#define         SERVICEPEND         (*(volatile unsigned int *) (0x7F008288))

/* EXTERN PIN CONFIGURATION REGISTER IN SLEEP MODE */
#define         SPCONSLP            (*(volatile unsigned int *) (0x7F008880))
#define         SLPEN               (*(volatile unsigned int *) (0x7F008930))

/*******************************************************************************
*
* 14 - Display Controller
*
*******************************************************************************/

/* VIDEO MAIN CONTROL REGISTERS */
#define         VIDCON0             (*(volatile unsigned int *) (0x77100000))
#define         VIDCON1             (*(volatile unsigned int *) (0x77100004))
#define         VIDCON2             (*(volatile unsigned int *) (0x77100008))

/* VIDEO TIME CONTROL REGISTERS */
#define         VIDTCON0            (*(volatile unsigned int *) (0x77100010))
#define         VIDTCON1            (*(volatile unsigned int *) (0x77100014))
#define         VIDTCON2            (*(volatile unsigned int *) (0x77100018))

/* WINDOW CONTROL REGISTERS */
#define         WINCON0             (*(volatile unsigned int *) (0x77100020))
#define         WINCON1             (*(volatile unsigned int *) (0x77100024))
#define         WINCON2             (*(volatile unsigned int *) (0x77100028))
#define         WINCON3             (*(volatile unsigned int *) (0x7710002C))
#define         WINCON4             (*(volatile unsigned int *) (0x77100030))

/* WINDOW POSITION CONTROL REGISTERS */
#define         VIDOSD0A            (*(volatile unsigned int *) (0x77100040))
#define         VIDOSD0B            (*(volatile unsigned int *) (0x77100044))
#define         VIDOSD0C            (*(volatile unsigned int *) (0x77100048))

#define         VIDOSD1A            (*(volatile unsigned int *) (0x77100050))
#define         VIDOSD1B            (*(volatile unsigned int *) (0x77100054))
#define         VIDOSD1C            (*(volatile unsigned int *) (0x77100058))
#define         VIDOSD1D            (*(volatile unsigned int *) (0x7710005C))

#define         VIDOSD2A            (*(volatile unsigned int *) (0x77100060))
#define         VIDOSD2B            (*(volatile unsigned int *) (0x77100064))
#define         VIDOSD2C            (*(volatile unsigned int *) (0x77100068))
#define         VIDOSD2D            (*(volatile unsigned int *) (0x7710006C))

#define         VIDOSD3A            (*(volatile unsigned int *) (0x77100070))
#define         VIDOSD3B            (*(volatile unsigned int *) (0x77100074))
#define         VIDOSD3C            (*(volatile unsigned int *) (0x77100078))

#define         VIDOSD4A            (*(volatile unsigned int *) (0x77100080))
#define         VIDOSD4B            (*(volatile unsigned int *) (0x77100084))
#define         VIDOSD4C            (*(volatile unsigned int *) (0x77100088))


/* FRAME BUFFER ADDRESS REGISTERS */
#define         VIDW00ADD0B0        (*(volatile unsigned int *) (0x771000A0))
#define         VIDW00ADD0B1        (*(volatile unsigned int *) (0x771000A4))
#define         VIDW01ADD0B0        (*(volatile unsigned int *) (0x771000A8))
#define         VIDW01ADD0B1        (*(volatile unsigned int *) (0x771000AC))
#define         VIDW02ADD0          (*(volatile unsigned int *) (0x771000B0))
#define         VIDW03ADD0          (*(volatile unsigned int *) (0x771000B8))
#define         VIDW04ADD0          (*(volatile unsigned int *) (0x771000C0))

#define         VIDW00ADD1B0        (*(volatile unsigned int *) (0x771000D0))
#define         VIDW00ADD1B1        (*(volatile unsigned int *) (0x771000D4))
#define         VIDW01ADD1B0        (*(volatile unsigned int *) (0x771000D8))
#define         VIDW01ADD1B1        (*(volatile unsigned int *) (0x771000DC))
#define         VIDW02ADD1          (*(volatile unsigned int *) (0x771000E0))
#define         VIDW03ADD1          (*(volatile unsigned int *) (0x771000E8))
#define         VIDW04ADD1          (*(volatile unsigned int *) (0x771000F0))

#define         VIDW00ADD2          (*(volatile unsigned int *) (0x77100100))
#define         VIDW01ADD2          (*(volatile unsigned int *) (0x77100104))
#define         VIDW02ADD2          (*(volatile unsigned int *) (0x77100108))
#define         VIDW03ADD2          (*(volatile unsigned int *) (0x7710010C))
#define         VIDW04ADD2          (*(volatile unsigned int *) (0x77100110))

/* VIDEO INTERRUPT CONTROL REGISTERS */
#define         VIDINTCON0          (*(volatile unsigned int *) (0x77100130))
#define         VIDINTCON1          (*(volatile unsigned int *) (0x77100134))

/* WIN COLOR KEY REGISTERS */
#define         W1KEYCON0           (*(volatile unsigned int *) (0x77100140))
#define         W1KEYCON1           (*(volatile unsigned int *) (0x77100144))

#define         W2KEYCON0           (*(volatile unsigned int *) (0x77100148))
#define         W2KEYCON1           (*(volatile unsigned int *) (0x7710014C))

#define         W3KEYCON0           (*(volatile unsigned int *) (0x77100150))
#define         W3KEYCON1           (*(volatile unsigned int *) (0x77100154))

#define         W4KEYCON0           (*(volatile unsigned int *) (0x77100158))
#define         W4KEYCON1           (*(volatile unsigned int *) (0x7710015C))





/*******************************************************************************
* 23 - Modem Interface
*******************************************************************************/


/* MODEM INTERFACE PORT CONTROL REGISTER */
#define         MIFPCON             (*(volatile unsigned int *) (0x7410800C))


/*******************************************************************************
* 30 - IIC
*******************************************************************************/

/* MULTI-MASTER IIC-BUS CONTROL(IICCON) REGISTER */
#define         IICCON0             (*(volatile unsigned int *) (0x7F004000))
#define         IICCON1             (*(volatile unsigned int *) (0x7F00F000))

/* MULTI-MASTER IIC-BUS CONTROL/STATUS(IICSTAT) REGISTER */
#define         IICSTAT0            (*(volatile unsigned int *) (0x7F004004))
#define         IICSTAT1            (*(volatile unsigned int *) (0x7F00F004))

/* MULTI-MASTER IIC-BUS ADDRESS(IICADD) REGISTER */
#define         IICADD0             (*(volatile unsigned int *) (0x7F004008))
#define         IICADD1             (*(volatile unsigned int *) (0x7F00F008))

/* MULTI-MASTER IIC-BUS TRANSMIT/RECEIVE DATA SHIFT(IICDS) REGISTER */
#define         IICDS0              (*(volatile unsigned int *) (0x7F00400C))
#define         IICDS1              (*(volatile unsigned int *) (0x7F00F00C))

/* MULTI-MASTER IIC-BUS LINE CONTROL(IICLC) REGISTER */
#define         IICLC0              (*(volatile unsigned int *) (0x7F004010))
#define         IICLC1              (*(volatile unsigned int *) (0x7F00F010))


/*******************************************************************************
* 31 - UART
*******************************************************************************/

/* UART LINE CONTORL REGISTERS */
#define         ULCON0              (*(volatile unsigned int *) (0x7F005000))
#define         ULCON1              (*(volatile unsigned int *) (0x7F005400))
#define         ULCON2              (*(volatile unsigned int *) (0x7F005800))
#define         ULCON3              (*(volatile unsigned int *) (0x7F005C00))

/* UART CONTROL REGISTERS */
#define         UCON0               (*(volatile unsigned int *) (0x7F005004))
#define         UCON1               (*(volatile unsigned int *) (0x7F005404))
#define         UCON2               (*(volatile unsigned int *) (0x7F005804))
#define         UCON3               (*(volatile unsigned int *) (0x7F005C04))

/* UART FIFO CONTROL REGISTERS */
#define         UFCON0              (*(volatile unsigned int *) (0x7F005008))
#define         UFCON1              (*(volatile unsigned int *) (0x7F005408))
#define         UFCON2              (*(volatile unsigned int *) (0x7F005808))
#define         UFCON3              (*(volatile unsigned int *) (0x7F005C08))

/* UART MODEM CONTROL REGISTERS */
#define         UMCON0              (*(volatile unsigned int *) (0x7F00500C))
#define         UMCON1              (*(volatile unsigned int *) (0x7F00540C))

/* UART TX/RX STATUS REGISTERS */
#define         UTRSTAT0            (*(volatile unsigned int *) (0x7F005010))
#define         UTRSTAT1            (*(volatile unsigned int *) (0x7F005410))
#define         UTRSTAT2            (*(volatile unsigned int *) (0x7F005810))
#define         UTRSTAT3            (*(volatile unsigned int *) (0x7F005C10))

/* UART ERROR STATUS REGISTERS */
#define         UERSTAT0            (*(volatile unsigned int *) (0x7F005014))
#define         UERSTAT1            (*(volatile unsigned int *) (0x7F005414))
#define         UERSTAT2            (*(volatile unsigned int *) (0x7F005814))
#define         UERSTAT3            (*(volatile unsigned int *) (0x7F005C14))

/* UART FIFO STATUS REGISTERS */
#define         UFSTAT0             (*(volatile unsigned int *) (0x7F005018))
#define         UFSTAT1             (*(volatile unsigned int *) (0x7F005418))
#define         UFSTAT2             (*(volatile unsigned int *) (0x7F005818))
#define         UFSTAT3             (*(volatile unsigned int *) (0x7F005C18))

/* UART MODEM STATUS REGISTERS */
#define         UMSTAT0             (*(volatile unsigned int *) (0x7F00501C))
#define         UMSTAT1             (*(volatile unsigned int *) (0x7F00541C))

/* UART TRANSMIT BUFFER REGISTERS */
#define         UTXH0               (*(volatile unsigned int *) (0x7F005020))
#define         UTXH1               (*(volatile unsigned int *) (0x7F005420))
#define         UTXH2               (*(volatile unsigned int *) (0x7F005820))
#define         UTXH3               (*(volatile unsigned int *) (0x7F005C20))

/* UART RECIVE BUFFER REGISTERS */
#define         URXH0               (*(volatile unsigned int *) (0x7F005024))
#define         URXH1               (*(volatile unsigned int *) (0x7F005424))
#define         URXH2               (*(volatile unsigned int *) (0x7F005824))
#define         URXH3               (*(volatile unsigned int *) (0x7F005C24))

/* UART CLOCK AND PCLK RELATION */
#define         UBRDIV0             (*(volatile unsigned int *) (0x7F005028))
#define         UBRDIV1             (*(volatile unsigned int *) (0x7F005428))
#define         UBRDIV2             (*(volatile unsigned int *) (0x7F005828))
#define         UBRDIV3             (*(volatile unsigned int *) (0x7F005C28))

#define         UDIVSLOT0           (*(volatile unsigned int *) (0x7F00502C))
#define         UDIVSLOT1           (*(volatile unsigned int *) (0x7F00542C))
#define         UDIVSLOT2           (*(volatile unsigned int *) (0x7F00582C))
#define         UDIVSLOT3           (*(volatile unsigned int *) (0x7F005C2C))

/* UART INTERRUPT PENDING REGISTERS */
#define         UINTP0              (*(volatile unsigned int *) (0x7F005030))
#define         UINTP1              (*(volatile unsigned int *) (0x7F005430))
#define         UINTP2              (*(volatile unsigned int *) (0x7F005830))
#define         UINTP3              (*(volatile unsigned int *) (0x7F005C30))

/* UART INTERRUPT SOURCE PENDING REGISTERS */
#define         UINTSP0             (*(volatile unsigned int *) (0x7F005034))
#define         UINTSP1             (*(volatile unsigned int *) (0x7F005434))
#define         UINTSP2             (*(volatile unsigned int *) (0x7F005834))
#define         UINTSP3             (*(volatile unsigned int *) (0x7F005C34))

/* UART INTERRUPT MASK REGISTERS */
#define         UINTM0              (*(volatile unsigned int *) (0x7F005038))
#define         UINTM1              (*(volatile unsigned int *) (0x7F005438))
#define         UINTM2              (*(volatile unsigned int *) (0x7F005838))
#define         UINTM3              (*(volatile unsigned int *) (0x7F005C38))

/*******************************************************************************
* 32 - PWM TIMER
*******************************************************************************/



/*******************************************************************************
* 33 - RTC
*******************************************************************************/

/* REAL TIME CLOCK CONTROL(RTCCON) REGISTER */
#define         RTCCON              (*(volatile unsigned int *) (0x7E005040))

/* TICK TIME COUNT REGISTER(TICNT) */
#define         TICNT               (*(volatile unsigned int *) (0x7E005044))

/* RTC ALARM CONTROL(RTCALM) REGISTER */
#define         RTCALM              (*(volatile unsigned int *) (0x7E005050))

/* ALARM SECOND DATA(ALMSEC) REGISTER */
#define         ALMSEC              (*(volatile unsigned int *) (0x7E005054))

/* ALARM MIN DATA(ALMMIN) REGISTER */
#define         ALMMIN              (*(volatile unsigned int *) (0x7E005058))

/* ALARM HOUR DATA(ALMHOUR) REGISTER */
#define         ALMHOUR             (*(volatile unsigned int *) (0x7E00505C))

/* ALARM DATE DATA(ALMDATE) REGISTER */
#define         ALMDATE             (*(volatile unsigned int *) (0x7E005060))

/* ALARM MONTH DATA(ALMMON) REGISTER */
#define         ALMMON              (*(volatile unsigned int *) (0x7E005064))

/* ALARM YEAR DATA(ALMYEAR) REGISTER */
#define         ALMYEAR             (*(volatile unsigned int *) (0x7E005068))

/* BCD SECOND(BCDSEC) REGISTER */
#define         BCDSEC              (*(volatile unsigned int *) (0x7E005070))

/* BCD MINUTE(BCDMIN) REGISTER */
#define         BCDMIN              (*(volatile unsigned int *) (0x7E005074))

/* BCD HOUR(BCDHOUR) REGISTER */
#define         BCDHOUR             (*(volatile unsigned int *) (0x7E005078))

/* BCD DATE(BCDDATE) REGISTER */
#define         BCDDATE             (*(volatile unsigned int *) (0x7E00507C))

/* BCD DAY(BCDDAY) REGISTER */
#define         BCDDAY              (*(volatile unsigned int *) (0x7E005080))

/* BCD MONTH(BCDMON) REGISTER */
#define         BCDMON              (*(volatile unsigned int *) (0x7E005084))

/* BCD YEAR(BCDYEAR) REGISTER */
#define         BCDYEAR             (*(volatile unsigned int *) (0x7E005088))

/* TICK COUNTER REGISTER */
#define         CURTICCNT           (*(volatile unsigned int *) (0x7E005090))

/* INTERRUPT PENDING REGISTER */
#define         INTP                (*(volatile unsigned int *) (0x7E005030))

/*******************************************************************************
* 34 - WATCHDOG TIMER
*******************************************************************************/















#endif // REGISTERS_H