#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#
# uCGUI/GUI/Widget

# name for executable
EXE = Widget

# list of source files
SRC_S = 
SRC_C =                 \
        BUTTON.c        \
        BUTTON__SetBitmapObj.c  \
        BUTTON_Bitmap.c         \
        BUTTON_BMP.c    \
        BUTTON_Create.c \
        BUTTON_CreateIndirect.c \
        BUTTON_Default.c    \
        BUTTON_Get.c    \
        BUTTON_IsPressed.c  \
        BUTTON_SelfDraw.c   \
        BUTTON_SetTextAlign.c   \
        BUTTON_StreamedBitmap.c \
        CHECKBOX.c  \
        CHECKBOX_Create.c   \
        CHECKBOX_CreateIndirect.c   \
        CHECKBOX_Default.c  \
        CHECKBOX_GetState.c \
        CHECKBOX_Image.c    \
        CHECKBOX_IsChecked.c    \
        CHECKBOX_SetBkColor.c   \
        CHECKBOX_SetDefaultImage.c  \
        CHECKBOX_SetFont.c  \
        CHECKBOX_SetImage.c \
        CHECKBOX_SetNumStates.c \
        CHECKBOX_SetSpacing.c   \
        CHECKBOX_SetState.c \
        CHECKBOX_SetText.c  \
        CHECKBOX_SetTextAlign.c \
        CHECKBOX_SetTextColor.c \
        DIALOG.c    \
        DROPDOWN.c  \
        DROPDOWN_Create.c   \
        DROPDOWN_CreateIndirect.c   \
        DROPDOWN_DeleteItem.c   \
        DROPDOWN_InsertString.c \
        DROPDOWN_ItemSpacing.c  \
        DROPDOWN_SetAutoScroll.c    \
        DROPDOWN_SetTextAlign.c \
        DROPDOWN_SetTextHeight.c    \
        EDIT.c  \
        EDIT_Create.c   \
        EDIT_CreateIndirect.c   \
        EDIT_Default.c  \
        EDIT_GetNumChars.c  \
        EDIT_SetCursorAtChar.c  \
        EDIT_SetInsertMode.c    \
        EDIT_SetpfAddKeyEx.c    \
        EDIT_SetpfUpdateBuffer.c    \
        EDIT_SetSel.c   \
        EDITBin.c   \
        EDITDec.c   \
        EDITFloat.c \
        EDITHex.c   \
        FRAMEWIN.c  \
        FRAMEWIN__UpdateButtons.c   \
        FRAMEWIN_AddMenu.c  \
        FRAMEWIN_Button.c   \
        FRAMEWIN_ButtonClose.c  \
        FRAMEWIN_ButtonMax.c    \
        FRAMEWIN_ButtonMin.c    \
        FRAMEWIN_Create.c   \
        FRAMEWIN_CreateIndirect.c   \
        FRAMEWIN_Default.c  \
        FRAMEWIN_Get.c  \
        FRAMEWIN_IsMinMax.c \
        FRAMEWIN_MinMaxRest.c   \
        FRAMEWIN_SetBorderSize.c    \
        FRAMEWIN_SetColors.c    \
        FRAMEWIN_SetFont.c    \
        FRAMEWIN_SetResizeable.c    \
        FRAMEWIN_SetTitleHeight.c    \
        FRAMEWIN_SetTitleVis.c    \
        GUI_ARRAY.c    \
        GUI_ARRAY_DeleteItem.c    \
        GUI_ARRAY_InsertItem.c    \
        GUI_ARRAY_ResizeItem.c    \
        GUI_DRAW.c    \
        GUI_DRAW_BITMAP.c    \
        GUI_DRAW_BMP.c    \
        GUI_DRAW_Self.c    \
        GUI_DRAW_STREAMED.c    \
        GUI_EditBin.c    \
        GUI_EditDec.c    \
        GUI_EditFloat.c    \
        GUI_EditHex.c    \
        GUI_EditString.c    \
        GUI_HOOK.c    \
        HEADER.c    \
        HEADER__SetDrawObj.c    \
        HEADER_Bitmap.c    \
        HEADER_BMP.c    \
        HEADER_Create.c    \
        HEADER_CreateIndirect.c    \
        HEADER_StreamedBitmap.c    \
        LISTBOX.c    \
        LISTBOX_Create.c    \
        LISTBOX_CreateIndirect.c    \
        LISTBOX_Default.c    \
        LISTBOX_DeleteItem.c    \
        LISTBOX_Font.c    \
        LISTBOX_GetItemText.c    \
        LISTBOX_GetNumItems.c    \
        LISTBOX_InsertString.c    \
        LISTBOX_ItemDisabled.c    \
        LISTBOX_ItemSpacing.c    \
        LISTBOX_MultiSel.c    \
        LISTBOX_ScrollStep.c    \
        LISTBOX_SetAutoScroll.c    \
        LISTBOX_SetBkColor.c    \
        LISTBOX_SetOwner.c    \
        LISTBOX_SetOwnerDraw.c    \
        LISTBOX_SetScrollbarWidth.c    \
        LISTBOX_SetString.c    \
        LISTBOX_SetTextColor.c    \
        LISTVIEW.c    \
        LISTVIEW_Create.c    \
        LISTVIEW_CreateIndirect.c    \
        LISTVIEW_Default.c    \
        LISTVIEW_DeleteColumn.c    \
        LISTVIEW_DeleteRow.c    \
        LISTVIEW_GetBkColor.c    \
        LISTVIEW_GetFont.c    \
        LISTVIEW_GetHeader.c    \
        LISTVIEW_GetNumColumns.c    \
        LISTVIEW_GetNumRows.c    \
        LISTVIEW_GetSel.c    \
        LISTVIEW_GetTextColor.c    \
        LISTVIEW_SetBkColor.c    \
        LISTVIEW_SetColumnWidth.c    \
        LISTVIEW_SetFont.c    \
        LISTVIEW_SetGridVis.c    \
        LISTVIEW_SetItemColor.c    \
        LISTVIEW_SetItemText.c    \
        LISTVIEW_SetLBorder.c    \
        LISTVIEW_SetRBorder.c    \
        LISTVIEW_SetRowHeight.c    \
        LISTVIEW_SetSel.c    \
        LISTVIEW_SetTextAlign.c    \
        LISTVIEW_SetTextColor.c    \
        MENU.c    \
        MENU__FindItem.c    \
        MENU_Attach.c    \
        MENU_CreateIndirect.c    \
        MENU_Default.c    \
        MENU_DeleteItem.c    \
        MENU_DisableItem.c    \
        MENU_EnableItem.c    \
        MENU_GetItem.c    \
        MENU_GetItemText.c    \
        MENU_GetNumItems.c    \
        MENU_InsertItem.c    \
        MENU_Popup.c    \
        MENU_SetBkColor.c    \
        MENU_SetBorderSize.c    \
        MENU_SetFont.c    \
        MENU_SetItem.c    \
        MENU_SetTextColor.c    \
        MESSAGEBOX.c    \
        MULTIEDIT.c    \
        MULTIEDIT_Create.c    \
        MULTIEDIT_CreateIndirect.c    \
        MULTIPAGE.c    \
        MULTIPAGE_Create.c    \
        MULTIPAGE_CreateIndirect.c    \
        MULTIPAGE_Default.c    \
        PROGBAR.c    \
        PROGBAR_Create.c    \
        PROGBAR_CreateIndirect.c    \
        RADIO.c    \
        RADIO_Create.c    \
        RADIO_CreateIndirect.c    \
        RADIO_Default.c    \
        RADIO_Image.c    \
        RADIO_SetBkColor.c    \
        RADIO_SetDefaultImage.c    \
        RADIO_SetFont.c    \
        RADIO_SetGroupId.c    \
        RADIO_SetImage.c    \
        RADIO_SetText.c    \
        RADIO_SetTextColor.c    \
        SCROLLBAR.c    \
        SCROLLBAR_Create.c    \
        SCROLLBAR_CreateIndirect.c    \
        SCROLLBAR_Defaults.c    \
        SCROLLBAR_GetValue.c    \
        SCROLLBAR_SetWidth.c    \
        SLIDER.c    \
        SLIDER_Create.c    \
        SLIDER_CreateIndirect.c    \
        TEXT.c    \
        TEXT_Create.c    \
        TEXT_CreateIndirect.c    \
        TEXT_SetBkColor.c    \
        TEXT_SetFont.c    \
        TEXT_SetText.c    \
        TEXT_SetTextAlign.c    \
        TEXT_SetTextColor.c    \
        WIDGET.c    \
        WIDGET_Effect_3D.c    \
        WIDGET_Effect_3D1L.c    \
        WIDGET_Effect_3D2L.c    \
        WIDGET_Effect_None.c    \
        WIDGET_Effect_Simple.c    \
        WIDGET_FillStringInRect.c    \
        WIDGET_SetEffect.c    \
        WIDGET_SetWidth.c    \
        WINDOW.c    \
        WINDOW_Default.c

# automatically generated list of object files
OBJS = $(SRC_S:.s=.o) $(SRC_C:.c=.o)

# compiler to use
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
CP = arm-none-linux-gnueabi-objcopy
DP = arm-none-linux-gnueabi-objdump

# include path
INCLUDE = -I$(shell pwd)/../Core    -I$(shell pwd)/../WM    \
          -I$(shell pwd)/../JPEG    -I$(shell pwd)/../MultiLayer    \
          -I$(shell pwd)/../Widget  \
          -I$(shell pwd)/../../Config

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer $(INCLUDE) -c

# default target
$(EXE).a : $(OBJS)
	$(AR) -cr -o $@ $^
	cp *.o $(EXE).a ../../uCGUI_obj

# dependencies
%.o : %.s
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	rm -f core *.o *.a