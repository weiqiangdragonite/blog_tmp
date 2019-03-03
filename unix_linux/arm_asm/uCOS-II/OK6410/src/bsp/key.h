/*******************************************************************************
* File: key.h
* 
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-14
*******************************************************************************/


#ifndef BSP_KEY_H
#define BSP_KEY_H


/* function prorotype */
void init_key(void);
void key_isr(void);

void key_1_handler(void);
void key_2_handler(void);
void key_3_handler(void);
void key_4_handler(void);
void key_5_handler(void);
void key_6_handler(void);


#endif  // BSP_KEY_H