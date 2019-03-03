

        .text
_start: .global _start


        ldr r0, [sp, #4]!
        add r1, sp, #4
        
        bl main
        @bl _exit
    
halt:
        b halt
        
        .end
        