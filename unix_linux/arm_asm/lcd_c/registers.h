/*******************************************************************************
* File: registers.h
* For: S3C6410
* 
* Description:
*   Define all the registers address.
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-16
*******************************************************************************/

#ifndef REGISTERS_H
#define REGISTERS_H


/*******************************************************************************
*
* 10 - GPIO
*
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
*
* 23 - Modem Interface
*
*******************************************************************************/


/* MODEM INTERFACE PORT CONTROL REGISTER */
#define         MIFPCON             (*(volatile unsigned int *) (0x7410800C))


/*******************************************************************************
*
* 31 - UART
*
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
*
* 32 - PWM TIMER
*
*******************************************************************************/





#endif // REGISTERS_H