/*
*********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*
*                             (c) Copyright 1992-2004, Micrium, Weston, FL
*                                          All Rights Reserved
*
*                                           ARM Generic Port
*                                             GCC Compiler
*
* File    : OS_CPU.H
* Version : V1.50
* By      : Jean J. Labrosse
*
* Modified by  : weiqiangdragonite@gmail.com
* Last updated : 2013-10-5
*********************************************************************************************************
*/


#ifndef OS_CPU_H
#define OS_CPU_H


// OS_CPU_GLOBALS and OS_CPU_EXT allows us to declare global variables that are specific to this port
#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif


/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bit wide                    */
typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */


#define TRUE           1
#define true           1
#define FALSE          0
#define false          0

#ifndef NULL
#define NULL           0
#endif


/*
*********************************************************************************************************
*                                                ARM
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
*             will be enabled even if they were disabled before entering the critical section.
*             NOT IMPLEMENTED
*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if 
*             interrupts were disabled before entering the critical section, they will be disabled when
*             leaving the critical section.
*             NOT IMPLEMENTED
*
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to 
*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
*             into the CPU's status register.
*********************************************************************************************************
*/


#define  OS_CRITICAL_METHOD    3


#if      OS_CRITICAL_METHOD == 3

// allows us to measure the amount of time interrupts are disabled.
#if      OS_CPU_INT_DIS_MEAS_EN > 0

#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();  \
                               OS_CPU_IntDisMeasStart();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_IntDisMeasStop();   \
                               OS_CPU_SR_Restore(cpu_sr);}

#else

#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}

#endif

#endif


/*
*********************************************************************************************************
*                                         ARM Miscellaneous
*********************************************************************************************************
*/

#define  OS_STK_GROWTH        1                   /* Stack grows from HIGH to LOW memory on ARM        */

#define  OS_TASK_SW()         OSCtxSw()


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

/* Variables used to measure interrupt disable time  */
#if OS_CPU_INT_DIS_MEAS_EN > 0
OS_CPU_EXT  INT16U  OS_CPU_IntDisMeasNestingCtr;
OS_CPU_EXT  INT16U  OS_CPU_IntDisMeasCntsEnter;
OS_CPU_EXT  INT16U  OS_CPU_IntDisMeasCntsExit;
OS_CPU_EXT  INT16U  OS_CPU_IntDisMeasCntsMax;
OS_CPU_EXT  INT16U  OS_CPU_IntDisMeasCntsDelta;
OS_CPU_EXT  INT16U  OS_CPU_IntDisMeasCntsOvrhd;
#endif

/*
*********************************************************************************************************
*                                              PROTOTYPES
*********************************************************************************************************
*/

#if OS_CRITICAL_METHOD == 3                       /* Allocate storage for CPU status register          */
OS_CPU_SR  OS_CPU_SR_Save(void);
void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
#endif

// the ISR entry point for the IRQ/FIQ interrupt and is written in assembly language.
void       OS_CPU_IRQ_ISR(void);                  /* See OS_CPU_A.S                                    */
void       OS_CPU_FIQ_ISR(void);

// actually done by OS_CPU_XXX_ISR_Handler() which is written in C.
void       OS_CPU_IRQ_ISR_Handler(void);          /* in the BSP */
void       OS_CPU_FIQ_ISR_Handler(void);

// functions used to measure the interrupt disable time
// Basically, we read the value of a timer just after disabling interrupts and
// read it again before enabling interrupts. The difference in timer counts
// indicates the amount of time interrupts were disabled. OS_CPU_IntDisMeasStop()
// actually keeps track of the highest value of this delta counts and thus,
// the maximum interrupt disable time.
#if OS_CPU_INT_DIS_MEAS_EN > 0
void       OS_CPU_IntDisMeasInit(void);             /* in the os_cpu_c.c */
void       OS_CPU_IntDisMeasStart(void);
void       OS_CPU_IntDisMeasStop(void);
INT16U     OS_CPU_IntDisMeasTmrRd(void);            /* in the BSP, not figure out yet */
#endif

#endif  // OS_CPU_H