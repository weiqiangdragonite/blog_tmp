/*
;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;
;                             (c) Copyright 1992-2004, Micrium, Weston, FL
;                                          All Rights Reserved
;
;                                           ARM Generic Port
;                                             GCC Compiler
;
; File    : OS_CPU_A.ASM
; Version : V1.50
; By      : Jean J. Labrosse
;
; Modified by  : weiqiangdragonite@gmail.com
; Last updated : 2013-10-5
;********************************************************************************************************
*/


        .global OS_CPU_SR_Save
        .global OS_CPU_SR_Restore
        .global OSStartHighRdy
        .global OSCtxSw
        .global OSIntCtxSw
        .global OS_CPU_IRQ_ISR
        .global OS_CPU_FIQ_ISR


.equ    NO_INT,     0xC0        @ disable interrupts (both FIR and IRQ)
.equ    SYS32_MODE, 0x1F        @ enable IRQ and FIQ
.equ    FIQ32_MODE, 0x11
.equ    IRQ32_MODE, 0x12

/*
**********************************************************************************************************
;                                   CRITICAL SECTION METHOD 3 FUNCTIONS
;
; Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
;              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
;              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to 
;              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
;              into the CPU's status register.
;
; Prototypes :     OS_CPU_SR  OS_CPU_SR_Save(void);
;                  void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
;
;
; Note(s)    : 1) These functions are used in general like this:
;
;                 void Task (void *p_arg)
;                 {
;                 #if OS_CRITICAL_METHOD == 3          // Allocate storage for CPU status register
;                     OS_CPU_SR  cpu_sr;
;                 #endif
;
;                          :
;                          :
;                     OS_ENTER_CRITICAL();             // cpu_sr = OS_CPU_SaveSR();
;                          :
;                          :
;                     OS_EXIT_CRITICAL();              // OS_CPU_RestoreSR(cpu_sr);
;                          :
;                          :
;                 }
;
;              2) OS_CPU_SaveSR() is implemented as recommended by Atmel's application note:
;
;                    "Disabling Interrupts at Processor Level"
;*********************************************************************************************************
*/


OS_CPU_SR_Save:
        MRS     R0,CPSR                     @ Set IRQ and FIQ bits in CPSR to disable all interrupts
        ORR     R1,R0,#NO_INT
        MSR     CPSR_c,R1
        MRS     R1,CPSR                     @ Confirm that CPSR contains the proper interrupt disable flags
        AND     R1,R1,#NO_INT
        CMP     R1,#NO_INT
        BNE     OS_CPU_SR_Save              @ Not properly disabled (try again)
        MOV     PC,LR                       @ Disabled, return the original CPSR contents in R0


OS_CPU_SR_Restore:
        MSR     CPSR_c,R0
        MOV     PC,LR

/*
;*********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; Note(s) : 1) OSStartHighRdy() MUST:
;              a) Call OSTaskSwHook() then,
;              b) Set OSRunning to TRUE,
;              c) Switch to the highest priority task.
;*********************************************************************************************************
*/

@ called by OSStart() to start running the highest priority task that was
@ created before calling OSStart(). OSStart() sets OSTCBHighRdy to point to
@ the OS_TCB of the highest priority task.
OSStartHighRdy:

        MSR     CPSR_cxsf, #0xDF        @ Switch to SYS mode with IRQ and FIQ disabled
        
        BL      OSTaskSwHook            @ OSTaskSwHook();

        @ The μC/OS-II flag OSRunning is set to TRUE indicating that μC/OS-II
        @ will be running once the first task is started.
        LDR     R4, OS_Running          @ set: OSRunning = TRUE
        MOV     R5, #1
        STRB    R5, [R4]

        
                                        @ SWITCH TO HIGHEST PRIORITY TASK
        LDR     R4, OS_TCBHighRdy       @    Get highest priority task TCB address
        LDR     R4, [R4]                @    get stack pointer
        LDR     SP, [R4]                @    switch to the new stack

        @ We then pop the remaining registers of the task’s context from the
        @ stack. You should note that we DON’T use the LDMFD instruction.
        @ The reason is that the 'ARM Architecture Reference Manual, Second Edition' 
        @ specifically mentions that using these instructions in User or System
        @ mode leads to UNPREDICTABLE results. We thus decided to avoid these instructions.
        LDR     R4,  [SP], #4           @    pop new task's CPSR
        MSR     CPSR_cxsf,R4
        LDR     R0,  [SP], #4           @    pop new task's context
        LDR     R1,  [SP], #4
        LDR     R2,  [SP], #4
        LDR     R3,  [SP], #4
        LDR     R4,  [SP], #4
        LDR     R5,  [SP], #4
        LDR     R6,  [SP], #4
        LDR     R7,  [SP], #4
        LDR     R8,  [SP], #4
        LDR     R9,  [SP], #4
        LDR     R10, [SP], #4
        LDR     R11, [SP], #4
        LDR     R12, [SP], #4
        LDR     LR,  [SP], #4
        LDR     PC,  [SP], #4

/*
;*********************************************************************************************************
;                         PERFORM A CONTEXT SWITCH (From task level) - OSCtxSw()
;
; Note(s) : 1) OSCtxSw() is called in SYS mode with BOTH FIQ and IRQ interrupts DISABLED
;
;           2) The pseudo-code for OSCtxSw() is:
;              a) Save the current task's context onto the current task's stack
;              b) OSTCBCur->OSTCBStkPtr = SP;
;              c) OSTaskSwHook();
;              d) OSPrioCur             = OSPrioHighRdy;
;              e) OSTCBCur              = OSTCBHighRdy;
;              f) SP                    = OSTCBHighRdy->OSTCBStkPtr;
;              g) Restore the new task's context from the new task's stack
;              h) Return to new task's code
;
;           3) Upon entry: 
;              OSTCBCur      points to the OS_TCB of the task to suspend
;              OSTCBHighRdy  points to the OS_TCB of the task to resume
;*********************************************************************************************************
*/

@ The code to perform a ‘task level’ context switch.
@ OSCtxSw() is called when a higher priority task is made ready to run by
@ another task or, when the current task can no longer execute (e.g.
@ it calls OSTimeDly(), OSSemPend() and the semaphore is not available, etc.).
@ Recall that all tasks run in SYS mode. A task level context switch simply
@ consist of saving the SYS registers on the task to suspend and restore the
@ SYS registers of the new task
OSCtxSw:
                                        @ SAVE CURRENT TASK'S CONTEXT
        STR     LR,  [SP, #-4]!         @     Return address
        STR     LR,  [SP, #-4]!
        STR     R12, [SP, #-4]!
        STR     R11, [SP, #-4]!
        STR     R10, [SP, #-4]!
        STR     R9,  [SP, #-4]!
        STR     R8,  [SP, #-4]!
        STR     R7,  [SP, #-4]!
        STR     R6,  [SP, #-4]!
        STR     R5,  [SP, #-4]!
        STR     R4,  [SP, #-4]!
        STR     R3,  [SP, #-4]!
        STR     R2,  [SP, #-4]!
        STR     R1,  [SP, #-4]!
        STR     R0,  [SP, #-4]!
        MRS     R4,  CPSR               @    push current CPSR
        STR     R4,  [SP, #-4]!
        
        LDR     R4, OS_TCBCur           @ OSTCBCur->OSTCBStkPtr = SP;
        LDR     R5, [R4]
        STR     SP, [R5]

        BL      OSTaskSwHook            @ OSTaskSwHook();

        LDR     R4, OS_PrioCur          @ OSPrioCur = OSPrioHighRdy
        LDR     R5, OS_PrioHighRdy
        LDRB    R6, [R5]
        STRB    R6, [R4]
        
        LDR     R4, OS_TCBCur           @ OSTCBCur  = OSTCBHighRdy;
        LDR     R6, OS_TCBHighRdy
        LDR     R6, [R6]
        STR     R6, [R4]

        LDR     SP, [R6]                @ SP = OSTCBHighRdy->OSTCBStkPtr;

                                        @ RESTORE NEW TASK'S CONTEXT
        LDR     R4,  [SP], #4           @    pop new task's CPSR
        MSR     CPSR_cxsf, R4
        LDR     R0,  [SP], #4           @    pop new task's context
        LDR     R1,  [SP], #4
        LDR     R2,  [SP], #4
        LDR     R3,  [SP], #4
        LDR     R4,  [SP], #4
        LDR     R5,  [SP], #4
        LDR     R6,  [SP], #4
        LDR     R7,  [SP], #4
        LDR     R8,  [SP], #4
        LDR     R9,  [SP], #4
        LDR     R10, [SP], #4
        LDR     R11, [SP], #4
        LDR     R12, [SP], #4
        LDR     LR,  [SP], #4
        LDR     PC,  [SP], #4

/*
;*********************************************************************************************************
;                   PERFORM A CONTEXT SWITCH (From interrupt level) - OSIntCtxSw()
;
; Note(s) : 1) OSIntCtxSw() is called in SYS mode with BOTH FIQ and IRQ interrupts DISABLED
;
;           2) The pseudo-code for OSCtxSw() is:
;              a) OSTaskSwHook();
;              b) OSPrioCur             = OSPrioHighRdy;
;              c) OSTCBCur              = OSTCBHighRdy;
;              d) SP                    = OSTCBHighRdy->OSTCBStkPtr;
;              e) Restore the new task's context from the new task's stack
;              f) Return to new task's code
;
;           3) Upon entry: 
;              OSTCBCur      points to the OS_TCB of the task to suspend
;              OSTCBHighRdy  points to the OS_TCB of the task to resume
;*********************************************************************************************************
*/


@ When an ISR completes, OSIntExit() is called to determine whether a more
@ important task than the interrupted task needs to execute. If that’s the case,
@ OSIntExit() determines which task to run next and calls OSIntCtxSw() to
@ perform the actual context switch to that task.
OSIntCtxSw:
        BL      OSTaskSwHook            @ OSTaskSwHook();

        LDR     R4,OS_PrioCur           @ OSPrioCur = OSPrioHighRdy
        LDR     R5,OS_PrioHighRdy
        LDRB    R6,[R5]
        STRB    R6,[R4]
        
        LDR     R4,OS_TCBCur            @ OSTCBCur  = OSTCBHighRdy;
        LDR     R6,OS_TCBHighRdy
        LDR     R6,[R6]
        STR     R6,[R4]

        LDR     SP,[R6]                 @ SP = OSTCBHighRdy->OSTCBStkPtr;

                                        @ RESTORE NEW TASK'S CONTEXT
        LDR     R4,  [SP], #4           @    pop new task's CPSR
        MSR     CPSR_cxsf, R4
        LDR     R0,  [SP], #4           @    pop new task's context
        LDR     R1,  [SP], #4
        LDR     R2,  [SP], #4
        LDR     R3,  [SP], #4
        LDR     R4,  [SP], #4
        LDR     R5,  [SP], #4
        LDR     R6,  [SP], #4
        LDR     R7,  [SP], #4
        LDR     R8,  [SP], #4
        LDR     R9,  [SP], #4
        LDR     R10, [SP], #4
        LDR     R11, [SP], #4
        LDR     R12, [SP], #4
        LDR     LR,  [SP], #4
        LDR     PC,  [SP], #4

/*
;*********************************************************************************************************
;                                      IRQ Interrupt Service Routine
;*********************************************************************************************************
*/

@ may be udestand
OS_CPU_IRQ_ISR:

        @ now we are in the IRQ mode, use IRQ sp
        
        STR     R3, [SP, #-4]!                 @ PUSH WORKING REGISTERS ONTO IRQ STACK
        STR     R2, [SP, #-4]!
        STR     R1, [SP, #-4]!
        
        MOV     R1, SP                         @ Save   IRQ stack pointer
        
        ADD     SP, SP, #12                    @ Adjust IRQ stack pointer
                                               @ (back to the IRQ Stack Start)
        
        @ modify the return address: R2 = LR - 4
        SUB     R2, LR, #4                     @ Adjust PC for return address to task

        MRS     R3, SPSR                       @ Copy SPSR (i.e. interrupted task's CPSR) to R3
        
        
        /**********************************************************************/
        
        MSR     CPSR_c, #(NO_INT | SYS32_MODE) @ Change to SYS mode

        @ now we are in the SYS mode, use USR/SYS sp
        @ R1 = IRQ_sp(have push R1, R2, R3)
        @ R2 = LR - 4
        @ R3 = SPSR
                                               @ SAVE TASK'S CONTEXT ONTO TASK'S STACK
        STR     R2,  [SP, #-4]!                @    Push task's Return PC 
        STR     LR,  [SP, #-4]!                @    Push task's LR
        STR     R12, [SP, #-4]!                @    Push task's R12-R4
        STR     R11, [SP, #-4]!
        STR     R10, [SP, #-4]!
        STR     R9,  [SP, #-4]!
        STR     R8,  [SP, #-4]!
        STR     R7,  [SP, #-4]!
        STR     R6,  [SP, #-4]!
        STR     R5,  [SP, #-4]!
        STR     R4,  [SP, #-4]!
        
        LDR     R4,  [R1], #4                  @    Move task's R1-R3(original) from IRQ stack to SYS stack
        LDR     R5,  [R1], #4        
        LDR     R6,  [R1], #4
        STR     R6,  [SP, #-4]!
        STR     R5,  [SP, #-4]!
        STR     R4,  [SP, #-4]!
        
        STR     R0,  [SP, #-4]!                @    Push task's R0    onto task's stack
        STR     R3,  [SP, #-4]!                @    Push task's CPSR (i.e. IRQ's SPSR)
                
        @ save the register end
        /**********************************************************************/
        
                                               @ HANDLE NESTING COUNTER
        LDR     R0, OS_IntNesting              @ OSIntNesting++;
                                               @ or call: OSIntEnter(); more safer but use more resource
        LDRB    R1, [R0]
        ADD     R1, R1,#1
        STRB    R1, [R0]
        

        CMP     R1, #1                         @ if (OSIntNesting == 1) {
        BNE     OS_CPU_IRQ_ISR_1

        LDR     R4, OS_TCBCur                  @     OSTCBCur->OSTCBStkPtr = SP
        LDR     R5, [R4]
        STR     SP, [R5]                       @ }

OS_CPU_IRQ_ISR_1:
        MSR     CPSR_c, #(NO_INT | IRQ32_MODE) @ Change to IRQ mode (to use the IRQ stack to handle interrupt)
        
        @ go to the ISR handler to clear interrupt
        BL      OS_CPU_IRQ_ISR_Handler         @ OS_CPU_IRQ_ISR_Handler();
        
        
        MSR     CPSR_c, #(NO_INT | SYS32_MODE) @ Change to SYS mode
        
        @ we can open interrupt here to enable higher priority interrupt
        
        BL      OSIntExit                     @ OSIntExit();
        
        
        /**********************************************************************/
        @ we are in the SYS mode
                                              @ RESTORE TASK'S CONTEXT and RETURN TO TASK
        LDR     R4,  [SP], #4                 @    pop new task's CPSR
        MSR     CPSR_cxsf, R4
        LDR     R0,  [SP], #4                 @    pop new task's context
        LDR     R1,  [SP], #4
        LDR     R2,  [SP], #4
        LDR     R3,  [SP], #4
        LDR     R4,  [SP], #4
        LDR     R5,  [SP], #4
        LDR     R6,  [SP], #4
        LDR     R7,  [SP], #4
        LDR     R8,  [SP], #4
        LDR     R9,  [SP], #4
        LDR     R10, [SP], #4
        LDR     R11, [SP], #4
        LDR     R12, [SP], #4
        LDR     LR,  [SP], #4
        LDR     PC,  [SP], #4

/*
;*********************************************************************************************************
;                                      FIQ Interrupt Service Routine
;*********************************************************************************************************
*/


OS_CPU_FIQ_ISR:
                                        
        STMFD   SP!,{R1-R3}                   @ PUSH WORKING REGISTERS ONTO IRQ STACK
        
        MOV     R1,SP                         @ Save   IRQ stack pointer
        
        ADD     SP,SP,#12                     @ Adjust FIQ stack pointer 
        
        SUB     R2,LR,#4                      @ Adjust PC for return address to task

        MRS     R3,SPSR                       @ Copy SPSR (i.e. interrupted task's CPSR) to R3
        
        MSR     CPSR_c,#(NO_INT | SYS32_MODE) @ Change to SYS mode

                                              @ SAVE TASK'S CONTEXT ONTO TASK'S STACK
        STMFD   SP!,{R2}                      @    Push task's Return PC 
        STMFD   SP!,{R4-R12,LR}               @    Push task's LR,R12-R4
        
        LDMFD   R1!,{R4-R6}                   @    Move task's R1-R3 from IRQ stack to SYS stack
        STMFD   SP!,{R4-R6}
        STMFD   SP!,{R0}                      @    Push task's R0    onto task's stack
        STMFD   SP!,{R3}                      @    Push task's CPSR (i.e. IRQ's SPSR)
        
        /**********************************************************************/
        
                                              @ HANDLE NESTING COUNTER
        LDR     R0,OS_IntNesting              @ OSIntNesting++;
        LDRB    R1,[R0]
        ADD     R1,R1,#1
        STRB    R1,[R0]

        CMP     R1,#1                         @ if (OSIntNesting == 1) {
        BNE     OS_CPU_FIQ_ISR_1

        LDR     R4,OS_TCBCur                  @     OSTCBCur->OSTCBStkPtr = SP
        LDR     R5,[R4]
        STR     SP,[R5]                       @ }

OS_CPU_FIQ_ISR_1:
        MSR     CPSR_c,#(NO_INT | FIQ32_MODE) @ Change to FIQ mode (to use the FIQ stack to handle interrupt)
        
        BL      OS_CPU_FIQ_ISR_Handler        @ OS_CPU_FIQ_ISR_Handler();
        
        MSR     CPSR_c,#(NO_INT | SYS32_MODE) @ Change to SYS mode
        
        BL      OSIntExit                     @ OSIntExit();
        
        /**********************************************************************/
        
                                              @ RESTORE TASK'S CONTEXT and RETURN TO TASK
        LDMFD   SP!,{R4}                      @ pop new task's CPSR
        MSR     CPSR_cxsf,r4
        LDMFD   SP!,{R0-R12,LR,PC}            @ pop new task's R0-R12,LR & PC

/*
;*********************************************************************************************************
;                                     POINTERS TO VARIABLES
;*********************************************************************************************************
*/

             
OS_IntNesting:
        .word    OSIntNesting

OS_PrioCur:
        .word    OSPrioCur

OS_PrioHighRdy:
        .word    OSPrioHighRdy

OS_Running:
        .word    OSRunning

OS_TCBCur:
        .word    OSTCBCur

OS_TCBHighRdy:
        .word    OSTCBHighRdy
        