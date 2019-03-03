/*******************************************************************************
* File: app.c
*
* By: weiqiangdragonite@gmail.com
* Last updated: 2013-10-8
*******************************************************************************/


#include "includes.h"

// uCGUI/Config
#include "../uCGUI/Config/GUIConf.h"
#include "../uCGUI/Config/GUITouchConf.h"
#include "../uCGUI/Config/LCDConf.h"

// uCGUI/GUI/Core
#include "../uCGUI/GUI/Core/GUI.h"
#include "../uCGUI/GUI/Core/GUI_ConfDefaults.h"
#include "../uCGUI/GUI/Core/GUI_FontIntern.h"
#include "../uCGUI/GUI/Core/GUI_Private.h"
#include "../uCGUI/GUI/Core/GUI_Protected.h"
#include "../uCGUI/GUI/Core/GUI_VNC.h"
#include "../uCGUI/GUI/Core/GUI_X.h"
#include "../uCGUI/GUI/Core/GUIDebug.h"
#include "../uCGUI/GUI/Core/GUIType.h"
#include "../uCGUI/GUI/Core/GUIVersion.h"
#include "../uCGUI/GUI/Core/LCD.h"
#include "../uCGUI/GUI/Core/LCD_ConfDefaults.h"
#include "../uCGUI/GUI/Core/LCD_Private.h"
#include "../uCGUI/GUI/Core/LCD_Protected.h"
#include "../uCGUI/GUI/Core/LCDSIM.h"

// uCGUI/GUI/JPEG
/*
#include "../uCGUI/GUI/JPEG/jconfig.h"
#include "../uCGUI/GUI/JPEG/jdct.h"
#include "../uCGUI/GUI/JPEG/jdhuff.h"
#include "../uCGUI/GUI/JPEG/jerror.h"
#include "../uCGUI/GUI/JPEG/jinclude.h"
#include "../uCGUI/GUI/JPEG/jmemsys.h"
#include "../uCGUI/GUI/JPEG/jmorecfg.h"
#include "../uCGUI/GUI/JPEG/jpegint.h"
#include "../uCGUI/GUI/JPEG/jpeglib.h"
#include "../uCGUI/GUI/JPEG/jversion.h"
*/

// uCGUI/GUI/MultiLayer
#include "../uCGUI/GUI/MultiLayer/LCD_IncludeDriver.h"

// uCGUI/GUI/Widget
#include "../uCGUI/GUI/Widget/BUTTON.h"
#include "../uCGUI/GUI/Widget/BUTTON_Private.h"
#include "../uCGUI/GUI/Widget/CHECKBOX.h"
#include "../uCGUI/GUI/Widget/CHECKBOX_Private.h"
#include "../uCGUI/GUI/Widget/DIALOG.h"
#include "../uCGUI/GUI/Widget/DIALOG_Intern.h"
#include "../uCGUI/GUI/Widget/DROPDOWN.h"
#include "../uCGUI/GUI/Widget/DROPDOWN_Private.h"
#include "../uCGUI/GUI/Widget/EDIT.h"
#include "../uCGUI/GUI/Widget/EDIT_Private.h"
#include "../uCGUI/GUI/Widget/FRAMEWIN.h"
#include "../uCGUI/GUI/Widget/FRAMEWIN_Private.h"
#include "../uCGUI/GUI/Widget/GUI_ARRAY.h"
#include "../uCGUI/GUI/Widget/GUI_HOOK.h"
#include "../uCGUI/GUI/Widget/HEADER.h"
#include "../uCGUI/GUI/Widget/HEADER_Private.h"
#include "../uCGUI/GUI/Widget/LISTBOX.h"
#include "../uCGUI/GUI/Widget/LISTBOX_Private.h"
#include "../uCGUI/GUI/Widget/LISTVIEW.h"
#include "../uCGUI/GUI/Widget/LISTVIEW_Private.h"
#include "../uCGUI/GUI/Widget/MENU.h"
#include "../uCGUI/GUI/Widget/MENU_Private.h"
#include "../uCGUI/GUI/Widget/MESSAGEBOX.h"
#include "../uCGUI/GUI/Widget/MULTIEDIT.h"
#include "../uCGUI/GUI/Widget/MULTIPAGE.h"
#include "../uCGUI/GUI/Widget/MULTIPAGE_Private.h"
#include "../uCGUI/GUI/Widget/PROGBAR.h"
#include "../uCGUI/GUI/Widget/RADIO.h"
#include "../uCGUI/GUI/Widget/RADIO_Private.h"
#include "../uCGUI/GUI/Widget/SCROLLBAR.h"
#include "../uCGUI/GUI/Widget/SCROLLBAR_Private.h"
#include "../uCGUI/GUI/Widget/SLIDER.h"
#include "../uCGUI/GUI/Widget/TEXT.h"
#include "../uCGUI/GUI/Widget/TEXT_Private.h"
#include "../uCGUI/GUI/Widget/WIDGET.h"
#include "../uCGUI/GUI/Widget/WINDOW_Private.h"

// uCGUI/GUI/WM
#include "../uCGUI/GUI/WM/WM.h"
#include "../uCGUI/GUI/WM/WM_GUI.h"
#include "../uCGUI/GUI/WM/WM_Intern.h"
#include "../uCGUI/GUI/WM/WM_Intern_ConfDep.h"


// task stack array
OS_STK MainTaskStk[TASK_STK_SIZE];

OS_STK HighTaskStk[TASK_STK_SIZE];
OS_STK MidTaskStk[TASK_STK_SIZE];
OS_STK LowTaskStk[TASK_STK_SIZE];

int time = 0;

extern void GUIDEMO_main(void);

int main(void)
{

uart_puts("in the main");
GUIDEMO_main();

/*
    // init uC/OS-II
    OSInit();
    
    // create main task(prio = 0)
    OSTaskCreate(MainTask, (void *) 0, &MainTaskStk[TASK_STK_SIZE - 1], 0);
    
    // start multiple tasks management
    OSStart();
*/
    
    return 0;
}

void MainTask(void *pdata)
{
    pdata = pdata;
    
    // init pwn clock
    init_pwm();
    
    // init stat stack
    OSStatInit();
    
    GUI_Init();
    
    // create task
    //OSTaskCreate(HighTask, (void *) 0, &HighTaskStk[TASK_STK_SIZE - 1], 5);
    //OSTaskCreate(MidTask, (void *) 0, &MidTaskStk[TASK_STK_SIZE - 1], 6);
    //OSTaskCreate(LowTask, (void *) 0, &LowTaskStk[TASK_STK_SIZE - 1], 7);
    
    // new line
    uart_puts("");
    
    // start task
    while (1) {
    
        if (OSTimeGet() <= 500) {
            GUI_SetBkColor(GUI_WHITE);
            GUI_Clear();
            
            uart_puts("white");
        } else if (OSTimeGet() > 500 && OSTimeGet() <= 1000) {
            GUI_SetBkColor(GUI_BLACK);
            GUI_Clear();
            
            uart_puts("black");
        } else if (OSTimeGet() > 1000 && OSTimeGet() <= 1500) {
            GUI_SetBkColor(GUI_RED);
            GUI_Clear();
            
            uart_puts("red");
        } else if (OSTimeGet() > 1500 && OSTimeGet() <= 2000) {
            GUI_SetBkColor(GUI_GREEN);
            GUI_Clear();
            
            uart_puts("green");
        } else if (OSTimeGet() > 2000 && OSTimeGet() <= 2500) {
            GUI_SetBkColor(GUI_BLUE);
            GUI_Clear();
            
            uart_puts("blue");
            OSTimeSet(0);
        }
    
        // delay 2 s
        OSTimeDlyHMSM(0, 0, 2, 0);
    }
}

void HighTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {

        for (int i = 0; i < 480; ++i) {
            TFTDotWrite(i, time, COLOR_RED);
        }
        ++time;
        
        // delay 1 s
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

void MidTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        
        // delay 2 s
        OSTimeDlyHMSM(0, 0, 2, 0);
    }
}

void LowTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        
        // delay 1 s
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}