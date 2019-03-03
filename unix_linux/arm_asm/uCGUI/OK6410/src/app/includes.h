/*******************************************************************************
* File: includes.h
*
* By: weiqiangdragonite@gmail.com
* Last updated: 2013-10-6
*******************************************************************************/


#ifndef INCLUDES_H
#define INCLUDES_H


// arm:
#include "../arm/os_cpu.h"

// kernel:
#include "../kernel/os_cfg.h"
#include "../kernel/ucos_ii.h"

// bsp:
#include "../bsp/registers.h"
#include "../bsp/uart.h"
#include "../bsp/time.h"
#include "../bsp/nand.h"
#include "../bsp/sdram.h"
#include "../bsp/led.h"
#include "../bsp/key.h"
#include "../bsp/lcd.h"

/*
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
*/


// app:
#include "../app/app.h"



#endif  // INCLUDES_H