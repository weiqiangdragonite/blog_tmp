
#ifndef KEY_H
#define KEY_H


#include "registers.h"

/* function prorotype */
void init_key(void);
void key_isr(void);
void key_1_handler(void);
void key_2_handler(void);
void key_3_handler(void);
void key_4_handler(void);
void key_5_handler(void);
void key_6_handler(void);


#endif  // KEY_H