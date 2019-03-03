#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#
# uCGUI/GUI/WM

# name for executable
EXE = WM

# list of source files
SRC_S = 
SRC_C =                 \
        WM.c    \
        WM__ForEachDesc.c    \
        WM__GetFirstSibling.c    \
        WM__GetFocussedChild.c    \
        WM__GetLastSibling.c    \
        WM__GetOrg_AA.c    \
        WM__GetPrevSibling.c    \
        WM__IsAncestor.c    \
        WM__IsChild.c    \
        WM__IsEnabled.c    \
        WM__NotifyVisChanged.c    \
        WM__Screen2Client.c    \
        WM__SendMessage.c    \
        WM__SendMessageIfEnabled.c    \
        WM__SendMessageNoPara.c    \
        WM__UpdateChildPositions.c    \
        WM_AttachWindow.c    \
        WM_BringToBottom.c    \
        WM_BringToTop.c    \
        WM_Broadcast.c    \
        WM_CheckScrollPos.c    \
        WM_CriticalHandle.c    \
        WM_DIAG.c    \
        WM_EnableWindow.c    \
        WM_ForEachDesc.c    \
        WM_GetBkColor.c    \
        WM_GetClientRect.c    \
        WM_GetClientWindow.c    \
        WM_GetDesktopWindow.c    \
        WM_GetDesktopWindowEx.c    \
        WM_GetDiagInfo.c    \
        WM_GetDialogItem.c    \
        WM_GetFirstChild.c    \
        WM_GetFlags.c    \
        WM_GetFocussedWindow.c    \
        WM_GetId.c    \
        WM_GetInsideRect.c    \
        WM_GetInsideRectExScrollbar.c    \
        WM_GetInvalidRect.c    \
        WM_GetNextSibling.c    \
        WM_GetOrg.c    \
        WM_GetParent.c    \
        WM_GetPrevSibling.c    \
        WM_GetScrollbar.c    \
        WM_GetScrollPartner.c    \
        WM_GetScrollState.c    \
        WM_GetWindowRect.c    \
        WM_GetWindowSize.c    \
        WM_HasCaptured.c    \
        WM_HasFocus.c    \
        WM_Hide.c    \
        WM_IsCompletelyVisible.c    \
        WM_IsEnabled.c    \
        WM_IsFocussable.c    \
        WM_IsVisible.c    \
        WM_IsWindow.c    \
        WM_MakeModal.c    \
        WM_Move.c    \
        WM_MoveChildTo.c    \
        WM_NotifyParent.c    \
        WM_OnKey.c    \
        WM_Paint.c    \
        WM_PID__GetPrevState.c    \
        WM_ResizeWindow.c    \
        WM_Screen2Win.c    \
        WM_SendMessageNoPara.c    \
        WM_SendToParent.c    \
        WM_SetAnchor.c    \
        WM_SetCallback.c    \
        WM_SetCapture.c    \
        WM_SetCaptureMove.c    \
        WM_SetCreateFlags.c    \
        WM_SetDesktopColor.c    \
        WM_SetFocus.c    \
        WM_SetFocusOnNextChild.c    \
        WM_SetFocusOnPrevChild.c    \
        WM_SetId.c    \
        WM_SetpfPollPID.c    \
        WM_SetScrollbar.c    \
        WM_SetScrollState.c    \
        WM_SetSize.c    \
        WM_SetTrans.c    \
        WM_SetTransState.c    \
        WM_SetUserClipRect.c    \
        WM_SetXSize.c    \
        WM_SetYSize.c    \
        WM_Show.c    \
        WM_StayOnTop.c    \
        WM_Timer.c    \
        WM_TimerExternal.c    \
        WM_UserData.c    \
        WM_Validate.c    \
        WM_ValidateWindow.c    \
        WMMemDev.c    \
        WMTouch.c

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