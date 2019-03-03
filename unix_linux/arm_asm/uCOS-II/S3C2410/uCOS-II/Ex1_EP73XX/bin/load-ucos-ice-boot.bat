@echo off
echo /**************************************************************************
echo *                                                                         *
echo *   PROJECT     : ARM port for UCOS-II                                    *
echo *                                                                         *
echo *   MODULE      : LOAD-UCOS-ICE-BOOT.bat                                  *
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
echo *   Batch file Flash downloader for ICE_BOOT for JTAG debug version.      *
echo *                                                                         *
echo **************************************************************************/
@echo on
download -p1 -b115200 ucos-ice-boot.bin