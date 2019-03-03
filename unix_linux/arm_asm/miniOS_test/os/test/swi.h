/*******************************************************************************
*
*******************************************************************************/

#ifndef SWI_H
#define SWI_H

// SWI number
#define SWI_ENTER_CRITICAL      0
#define SWI_EXIT_CRITICAL       1


// function pointer
typedef int (*func_ptr)(void);
extern func_ptr swi_table[];





#endif  // SWI_H