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


#define  OS_CPU_GLOBALS
#include "../app/includes.h"


#define  ARM_SYS_MODE   (0x0000001FL)   // enable IRQ and FIQ

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                            (BEGINNING)
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void  OSInitHookBegin (void)
{
}
#endif

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                               (END)
*
* Description: This function is called by OSInit() at the end of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/
// This function is called by μC/OS-II’s OSInit() at the very end of OSInit().
// It gives the opportunity to add additional initialization code specific to
// the port. In this case, we initialize global variables which are used by the
// interrupt disable measurement code (if OS_CPU_INT_DIS_MEAS_EN is not set to 0).
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void  OSInitHookEnd (void)
{
#if OS_CPU_INT_DIS_MEAS_EN > 0
    OS_CPU_IntDisMeasInit();
#endif
}
#endif

/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
// This function is called by μC/OS-II’s OSTaskCreate() or OSTaskCreateExt() when a task is created
#if OS_CPU_HOOKS_EN > 0 
void  OSTaskCreateHook (OS_TCB *ptcb)
{
// call the initialization function of μC/OS-View (an optional module available
// for μC/OS-II which performs task profiling at run-time.
// Note that for OSView_TaskCreateHook() to be called, the target resident
// code for μC/OS-View must be included as part of your build. In this case,
// you need to add a #define OS_VIEW_MODULE 1 in OS_CFG.H of your application.
#if OS_VIEW_MODULE > 0
    OSView_TaskCreateHook(ptcb);
#else
    (void)ptcb;                        /* Prevent compiler warning                                     */
#endif
}
#endif


/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 
void  OSTaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;                        /* Prevent compiler warning                                     */
}
#endif

/*
*********************************************************************************************************
*                                             IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do  
*              such things as STOP the CPU to conserve power.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are enabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION >= 251
void  OSTaskIdleHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                           STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-II's statistics task.  This allows your 
*              application to add functionality to the statistics task.
*
* Arguments  : none
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0 
void  OSTaskStatHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                        INITIALIZE A TASK'S STACK
*
* Description: This function is called by either OSTaskCreate() or OSTaskCreateExt() to initialize the
*              stack frame of the task being created.  This function is highly processor specific.
*
* Arguments  : task          is a pointer to the task code
*
*              p_arg         is a pointer to a user supplied data area that will be passed to the task
*                            when the task first executes.
*
*              ptos          is a pointer to the top of stack.  It is assumed that 'ptos' points to
*                            a 'free' entry on the task stack.  If OS_STK_GROWTH is set to 1 then 
*                            'ptos' will contain the HIGHEST valid address of the stack.  Similarly, if
*                            OS_STK_GROWTH is set to 0, the 'ptos' will contains the LOWEST valid address
*                            of the stack.
*
*              opt           specifies options that can be used to alter the behavior of OSTaskStkInit().
*                            (see uCOS_II.H for OS_TASK_OPT_???).
*
* Returns    : Always returns the location of the new top-of-stack' once the processor registers have
*              been placed on the stack in the proper order.
*
* Note(s)    : 1) Interrupts are enabled when your task starts executing. 
*              2) All tasks run in SYS mode.
*********************************************************************************************************
*/

// μC/OS-II assumes that tasks run in SYS mode (the CPSR of the task is
// initialized to ARM_SYS_MODE (0x1F)).(enable IRQ/FIQ)
OS_STK *OSTaskStkInit (void (*task)(void *pd), void *p_arg, OS_STK *ptos, INT16U opt)
{
    OS_STK *stk;


    opt      = opt;                         /* 'opt' is not used, prevent warning                      */
    stk      = ptos;                        /* Load stack pointer                                      */
    
    *(stk)   = (OS_STK)task;                /* Entry Point                                             */
    *(--stk) = (INT32U)0x14141414L;         /* R14 (LR)                                                */
    *(--stk) = (INT32U)0x12121212L;         /* R12                                                     */
    *(--stk) = (INT32U)0x11111111L;         /* R11                                                     */
    *(--stk) = (INT32U)0x10101010L;         /* R10                                                     */
    *(--stk) = (INT32U)0x09090909L;         /* R9                                                      */
    *(--stk) = (INT32U)0x08080808L;         /* R8                                                      */
    *(--stk) = (INT32U)0x07070707L;         /* R7                                                      */
    *(--stk) = (INT32U)0x06060606L;         /* R6                                                      */
    *(--stk) = (INT32U)0x05050505L;         /* R5                                                      */
    *(--stk) = (INT32U)0x04040404L;         /* R4                                                      */
    *(--stk) = (INT32U)0x03030303L;         /* R3                                                      */
    *(--stk) = (INT32U)0x02020202L;         /* R2                                                      */
    *(--stk) = (INT32U)0x01010101L;         /* R1                                                      */
    *(--stk) = (INT32U)p_arg;               /* R0 : argument                                           */
    *(--stk) = (INT32U)ARM_SYS_MODE;        /* CPSR  (Enable both IRQ and FIQ interrupts)              */

    return (stk);
}

/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the 
*                 task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/
#if (OS_CPU_HOOKS_EN > 0) && (OS_TASK_SW_HOOK_EN > 0)
void  OSTaskSwHook (void)
{
#if OS_VIEW_MODULE > 0
    OSView_TaskSwHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                           OS_TCBInit() HOOK
*
* Description: This function is called by OS_TCBInit() after setting up most of the TCB.
*
* Arguments  : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void  OSTCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;                                            /* Prevent Compiler warning                 */
}
#endif


/*
*********************************************************************************************************
*                                               TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

// call from OSTimeTick()
#if (OS_CPU_HOOKS_EN > 0) && (OS_TIME_TICK_HOOK_EN > 0)
unsigned int int_time = 0;
void  OSTimeTickHook (void)
{
#if OS_VIEW_MODULE > 0
    OSView_TickHook();
#endif

    /*
    if (int_time == 500) {
        uart_printf("\nEvery 500 ticks switch times = ");
        uart_print_int(OSCtxSwCtr);
        OSCtxSwCtr = 0;
        uart_printf("\ncpu usage = ");
        uart_print_int(OSCPUUsage);
        uart_puts("%");
        
        int_time = 0;
    }
    
    ++int_time;
    */

}
#endif



/*
*********************************************************************************************************
*                                               RETURN HOOK
*
* Description: This function is called
*
* Arguments  : 
*
* Note(s)    : 
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION >= 251
void  OSTaskReturnHook (OS_TCB *ptcb)
{
    (void)ptcb;                                            /* Prevent Compiler warning                 */
}
#endif


/*
*********************************************************************************************************
*                             INTERRUPT DISABLE TIME MEASUREMENT, START
*********************************************************************************************************
*/

#if OS_CPU_INT_DIS_MEAS_EN > 0

// called by OSInitHookEnd() to initialize the interrupt disable time measurement variables
void  OS_CPU_IntDisMeasInit (void)
{
    OS_CPU_IntDisMeasNestingCtr = 0;
    OS_CPU_IntDisMeasCntsEnter  = 0;
    OS_CPU_IntDisMeasCntsExit   = 0;
    OS_CPU_IntDisMeasCntsMax    = 0;
    OS_CPU_IntDisMeasCntsDelta  = 0;
    OS_CPU_IntDisMeasCntsOvrhd  = 0;
    OS_CPU_IntDisMeasStart();                              /* Measure the overhead of the functions    */
    OS_CPU_IntDisMeasStop();
    OS_CPU_IntDisMeasCntsOvrhd  = OS_CPU_IntDisMeasCntsDelta;
}

// OS_CPU_IntDisMeasStart() is called when interrupts are disabled by OS_ENTER_CRITICAL().
void  OS_CPU_IntDisMeasStart (void)
{
    OS_CPU_IntDisMeasNestingCtr++;
    if (OS_CPU_IntDisMeasNestingCtr == 1) {                /* Only measure at the first nested level   */
        OS_CPU_IntDisMeasCntsEnter = OS_CPU_IntDisMeasTmrRd();
    }
}

// OS_CPU_IntDisMeasStop() is called when interrupts are re-enabled by OS_EXIT_CRITICAL().
void  OS_CPU_IntDisMeasStop (void)
{
    OS_CPU_IntDisMeasNestingCtr--;                                         /* Decrement nesting ctr    */
    if (OS_CPU_IntDisMeasNestingCtr == 0) {
        OS_CPU_IntDisMeasCntsExit  = OS_CPU_IntDisMeasTmrRd();
        OS_CPU_IntDisMeasCntsDelta = OS_CPU_IntDisMeasCntsExit - OS_CPU_IntDisMeasCntsEnter;
        if (OS_CPU_IntDisMeasCntsDelta > OS_CPU_IntDisMeasCntsOvrhd) {     /* Ensure overhead < delta  */
            OS_CPU_IntDisMeasCntsDelta -= OS_CPU_IntDisMeasCntsOvrhd;
        } else {
            OS_CPU_IntDisMeasCntsDelta  = OS_CPU_IntDisMeasCntsOvrhd;
        }
        if (OS_CPU_IntDisMeasCntsDelta > OS_CPU_IntDisMeasCntsMax) {       /* Track MAXIMUM            */
            OS_CPU_IntDisMeasCntsMax = OS_CPU_IntDisMeasCntsDelta;
        }
    }
}
#endif
