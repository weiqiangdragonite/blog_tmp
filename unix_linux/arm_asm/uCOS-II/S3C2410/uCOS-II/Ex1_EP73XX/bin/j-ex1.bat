@echo off
echo /**************************************************************************
echo *                                                                         *
echo *   PROJECT     : ARM port for UCOS-II                                    *
echo *                                                                         *
echo *   MODULE      : J-EX1.bat                                               *
echo *                                                                         *
echo *   AUTHOR      : Michael Anburaj                                         *
echo *                 URL  : http://geocities.com/michaelanburaj/             *
echo *                 EMAIL: michaelanburaj@hotmail.com                       *
echo *                                                                         *
echo *   PROCESSOR   : EP7312 (32 bit ARM720T RISC core from CIRRUS Logic)     *
echo *                                                                         *
echo *   IDE         : SDT 2.51 Or ADS 1.2                                     *
echo *                                                                         *
echo *   DESCRIPTION :                                                         *
echo *   Batch file for Ex1 ARM_UCOS build JTAG debug version.                 *
echo *                                                                         *
echo **************************************************************************/
setlocal

rem *************************************************************************
rem * Build Configuration
rem *************************************************************************

rem *
rem * Target Mode
rem * Possible values: arm, armthumb
rem *
set TARGET=arm

rem *
rem * Target type
rem * Possible values: rom, jtag
rem *
set TARGET_TYPE=jtag

rem *
rem * Project name
rem *
set PROJ=j-ex1

rem *
rem * Output folder
rem *
set OUTPUT=.

rem *
rem * Tool chain
rem * possible values: ADS, SDT
rem *
set TOOLS=ADS

rem *
rem * Tool chain path
rem *
if %TOOLS% == ADS (
set TPATH=C:\Progra~1\ARM\ADSv1_2
)
if %TOOLS% == SDT (
set TPATH=C:\ARM251
)

rem *
rem * CPU byte order
rem * Possible values: LI, BI
rem *
set ENDIAN=LI

rem *
rem * Build flags
rem *
set C_BUILD_FLAGS=



rem *************************************************************************
rem * Build the image using the template
rem * Note: Do not modify below this line
rem *************************************************************************
CALL template\ex1.bat


rem *************************************************************************
rem * End Build
rem *************************************************************************
endlocal
@echo on