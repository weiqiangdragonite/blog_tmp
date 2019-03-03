

    .global OS_ENTER_CRITICAL
    .global OS_EXIT_CRITICAL
    .global sleep
    .global leds_on
    .global leds_off

    
.equ SWI_ENTER_CRITICAL,    0
.equ SWI_EXIT_CRITICAL,     1
.equ SWI_SLEEP,             3
.equ SWI_PRINT,             4
.equ SWI_LEDS_ON,           5
.equ SWI_LEDS_OFF,          6
.equ SWI_SET_LED,           7


OS_ENTER_CRITICAL:
    swi SWI_ENTER_CRITICAL
    mov pc, lr
    
OS_EXIT_CRITICAL:
    swi SWI_EXIT_CRITICAL
    mov pc, lr
    
    
sleep:
    @ r0 = time (ms)
    swi SWI_SLEEP
    mov pc, lr
    
leds_on:
    swi SWI_LEDS_ON
    mov pc, lr
    
leds_off:
    swi SWI_LEDS_OFF
    mov pc, lr