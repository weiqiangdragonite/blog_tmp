/*******************************************************************************
* File: app.h
*
* By: weiqiangdragonite@gmail.com
* Last updated: 2013-10-7
*******************************************************************************/


#ifndef APP_H
#define APP_H


// task stack size
#define TASK_STK_SIZE   128


// function prototype
void MainTask(void *pdata);
void FirstTask(void *pdata);
void SecondTask(void *pdata);


#endif  // APP_H