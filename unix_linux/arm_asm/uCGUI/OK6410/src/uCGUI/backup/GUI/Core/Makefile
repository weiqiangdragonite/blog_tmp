#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#
# uCGUI/GUI/Core

# name for executable
EXE = Core

# list of source files
SRC_S = 
SRC_C =                 \
        GUI2DLib.c      \
        GUI__AddSpaceHex.c  \
        GUI__CalcTextRect.c \
        GUI__DivideRound.c  \
        GUI__DivideRound32.c    \
        GUI__GetFontSizeY.c \
        GUI__GetNumChars.c  \
        GUI__HandleEOLine.c \
        GUI__IntersectRect.c    \
        GUI__IntersectRects.c   \
        GUI__memset.c   \
        GUI__memset16.c \
        GUI__Read.c \
        GUI__ReduceRect.c   \
        GUI__SetText.c  \
        GUI__strcmp.c   \
        GUI__strlen.c   \
        GUI__Wrap.c \
        GUI_AddBin.c    \
        GUI_AddDec.c    \
        GUI_AddDecMin.c \
        GUI_AddDecShift.c   \
        GUI_AddHex.c    \
        GUI_AddKeyMsgHook.c \
        GUI_ALLOC_AllocInit.c   \
        GUI_ALLOC_AllocZero.c   \
        GUI_BMP.c   \
        GUI_BMP_Serialize.c \
        GUI_CalcColorDist.c \
        GUI_ClearRectEx.c   \
        GUI_Color2VisColor.c    \
        GUI_CursorArrowL.c  \
        GUI_CursorArrowLI.c \
        GUI_CursorArrowLPx.c    \
        GUI_CursorArrowM.c  \
        GUI_CursorArrowMI.c \
        GUI_CursorArrowMPx.c    \
        GUI_CursorArrowS.c  \
        GUI_CursorArrowSI.c \
        GUI_CursorArrowSPx.c    \
        GUI_CursorCrossL.c  \
        GUI_CursorCrossLI.c \
        GUI_CursorCrossLPx.c     \
        GUI_CursorCrossM.c  \
        GUI_CursorCrossMI.c \
        GUI_CursorCrossMPx.c    \
        GUI_CursorCrossS.c  \
        GUI_CursorCrossSI.c \
        GUI_CursorCrossSPx.c    \
        GUI_CursorHeaderM.c \
        GUI_CursorHeaderMI.c    \
        GUI_CursorHeaderMPx.c   \
        GUI_CursorPal.c \
        GUI_CursorPalI.c    \
        GUI_DispBin.c   \
        GUI_DispCEOL.c  \
        GUI_DispChar.c  \
        GUI_DispChars.c \
        GUI_DispHex.c   \
        GUI_DispString.c    \
        GUI_DispStringAt.c  \
        GUI_DispStringAtCEOL.c  \
        GUI_DispStringHCenter.c \
        GUI_DispStringInRect.c  \
        GUI_DispStringInRectEx.c    \
        GUI_DispStringInRectMax.c   \
        GUI_DispStringLen.c \
        GUI_DrawBitmap.c    \
        GUI_DrawBitmapEx.c  \
        GUI_DrawBitmapExp.c \
        GUI_DrawBitmapMag.c \
        GUI_DrawFocusRect.c \
        GUI_DrawGraph.c \
        GUI_DrawHLine.c \
        GUI_DrawPie.c   \
        GUI_DrawPixel.c \
        GUI_DrawPolyline.c  \
        GUI_DrawRectEx.c    \
        GUI_DrawVLine.c \
        GUI_ErrorOut.c  \
        GUI_Exec.c  \
        GUI_FillPolygon.c   \
        GUI_FillRect.c  \
        GUI_FillRectEx.c    \
        GUI_GetBitmapPixelColor.c   \
        GUI_GetBitmapPixelIndex.c   \
        GUI_GetClientRect.c \
        GUI_GetColor.c  \
        GUI_GetDispPos.c    \
        GUI_GetFont.c   \
        GUI_GetFontInfo.c   \
        GUI_GetFontSizeY.c  \
        GUI_GetLineStyle.c  \
        GUI_GetStringDistX.c    \
        GUI_GetTextAlign.c  \
        GUI_GetTextExtend.c \
        GUI_GetTextMode.c   \
        GUI_GetVersionString.c  \
        GUI_GetYSizeOfFont.c    \
        GUI_Goto.c  \
        GUI_InitLUT.c   \
        GUI_InvertRect.c    \
        GUI_IsInFont.c  \
        GUI_Log.c   \
        GUI_MergeRect.c \
        GUI_MOUSE.c \
        GUI_MOUSE_DriverPS2.c   \
        GUI_MoveRect.c  \
        GUI_OnKey.c \
        GUI_Pen.c   \
        GUI_PID.c   \
        GUI_RectsIntersect.c    \
        GUI_SaveContext.c   \
        GUI_SelectLayer.c   \
        GUI_SelectLCD.c \
        GUI_SetColor.c  \
        GUI_SetColorIndex.c \
        GUI_SetDecChar.c    \
        GUI_SetDefault.c    \
        GUI_SetDrawMode.c   \
        GUI_SetFont.c   \
        GUI_SetLBorder.c    \
        GUI_SetLineStyle.c  \
        GUI_SetLUTColor.c   \
        GUI_SetLUTColorEx.c \
        GUI_SetLUTEntry.c   \
        GUI_SetOrg.c    \
        GUI_SetPixelIndex.c \
        GUI_SetTextAlign.c  \
        GUI_SetTextMode.c   \
        GUI_SetTextStyle.c  \
        GUI_SIF.c   \
        GUI_SIF_Prop.c  \
        GUI_TOUCH.c \
        GUI_TOUCH_DriverAnalog.c    \
        GUI_TOUCH_StoreState.c  \
        GUI_TOUCH_StoreUnstable.c   \
        GUI_UC.c    \
        GUI_UC_DispString.c \
        GUI_UC_EncodeNone.c \
        GUI_UC_EncodeUTF8.c \
        GUI_WaitKey.c   \
        GUI_WaitEvent.c \
        GUI_Warn.c  \
        GUIAlloc.c  \
        GUIArc.c    \
        GUIChar.c   \
        GUICharLine.c   \
        GUICharM.c  \
        GUICharP.c  \
        GUICirc.c   \
        GUIColor2Index.c    \
        GUICore.c   \
        GUICurs.c   \
        GUIEncJS.c  \
        GUIIndex2Color.c    \
        GUIPolyE.c  \
        GUIPolyM.c  \
        GUIPolyR.c  \
        GUIRealloc.c    \
        GUIStream.c \
        GUITask.c   \
        GUITime.c   \
        GUITimer.c  \
        GUIUC0.c    \
        GUIVal.c    \
        GUIValf.c   \
        LCD.c   \
        LCD_API.c   \
        LCD_DrawBitmap_565.c    \
        LCD_DrawBitmap_M565.c   \
        LCD_DrawVLine.c \
        LCD_GetColorIndex.c \
        LCD_GetEx.c \
        LCD_GetNumDisplays.c    \
        LCD_GetPixelColor.c \
        LCD_Index2ColorEx.c \
        LCD_L0_Generic.c    \
        LCD_MixColors256.c  \
        LCD_Mirror.c    \
        LCD_Rotate180.c \
        LCD_RotateCW.c  \
        LCD_RotateCCW.c \
        LCD_SelectLCD.c \
        LCD_SetClipRectEx.c \
        LCD_SetAPI.c    \
        LCD_UpdateColorIndices.c    \
        LCDAA.c \
        LCDColor.c  \
        LCDGetP.c   \
        LCDInfo.c   \
        LCDInfo0.c  \
        LCDInfo1.c  \
        LCDL0Delta.c    \
        LCDL0Mag.c  \
        LCDP1.c \
        LCDP565_Index2Color.c   \
        LCDPM565_Index2Color.c  \
        LCDRLE4.c   \
        LCDRLE8.c

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