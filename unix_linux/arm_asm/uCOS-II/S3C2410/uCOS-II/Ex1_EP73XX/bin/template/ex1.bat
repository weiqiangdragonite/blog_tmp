rem /**************************************************************************
rem *                                                                         *
rem *   PROJECT     : ARM port for UCOS-II                                    *
rem *                                                                         *
rem *   MODULE      : EX1.bat                                                 *
rem *                                                                         *
rem *   AUTHOR      : Michael Anburaj                                         *
rem *                 URL  : http://geocities.com/michaelanburaj/             *
rem *                 EMAIL: michaelanburaj@hotmail.com                       *
rem *                                                                         *
rem *   PROCESSOR   : EP7312 (32 bit ARM720T RISC core from CIRRUS Logic)     *
rem *                                                                         *
rem *   IDE         : SDT 2.51 Or ADS 1.2                                     *
rem *                                                                         *
rem *   DESCRIPTION :                                                         *
rem *   Batch file template for Ex1 ARM_UCOS build.                           *
rem *                                                                         *
rem **************************************************************************/


rem *************************************************************************
rem * Build settings (Compiler, Assembler & Linker flags)
rem * Note: Do not modify below this line
rem *************************************************************************
set BIN=%TPATH%\bin
set CC=%BIN%\armcc
if %TARGET% == arm (
set DCC=%BIN%\armcc
)
if %TARGET% == armthumb (
set DCC=%BIN%\tcc
)
set ASM=%BIN%\armasm
set LD=%BIN%\armlink
set PLD=%BIN%\fromelf
set A_BUILD_FLAGS=
if %TARGET_TYPE% == rom (
set DC_FLAGS=-O2
set LC_BUILD_FLAG=
set DA_FLAGS=
set DL_FLAGS=-ro-base 0x00000000 -rw-base 0x06000200
)
if %TARGET_TYPE% == jtag (
set DC_FLAGS=-O0 -g+ -Ddebug_build
set LC_BUILD_FLAG=
set DA_FLAGS=-PD "debug_build SETL {TRUE}" -g
set DL_FLAGS=-ro-base 0x0909c200 -rw-base 0x06000200
)
if %TARGET% == arm (
set AC_FLAGS=
set AA_FLAGS=
set AL_FLAGS=
)
if %TARGET% == armthumb (
set AC_FLAGS=-apcs /noswst/interwork -D__APCS_INTERWORK
set AA_FLAGS=
set AL_FLAGS=-info interwork
)
if %TOOLS% == ADS (
set LIB_PATH=
set SYMBOLS=-symbols
set ZEROPAD=
)
if %TOOLS% == SDT (
set LIB_PATH=-libpath %TPATH%\Lib\embedded
set SYMBOLS=-symbols - 
set ZEROPAD=-nozeropad
)
if %ENDIAN% == BI (
set ENDIAN_FLAG=-bigend
set E_FLAG_C=-D __BIG_ENDIAN
set E_FLAG_A=-pd "__BIG_ENDIAN SETL {TRUE}"
)
if %ENDIAN% == LI (
set ENDIAN_FLAG=-littleend
set E_FLAG_C= 
set E_FLAG_A=-pd "__BIG_ENDIAN SETL {FALSE}"
)
set C_FLAGS=-cpu ARM7TM %ENDIAN_FLAG% -c %DC_FLAGS% %E_FLAG_C% %AC_FLAGS% %C_BUILD_FLAGS% %LC_BUILD_FLAG% -D%TOOLS%
set A_FLAGS=%DA_FLAGS% %ENDIAN_FLAG% %E_FLAG_A% %AA_FLAGS% %A_BUILD_FLAGS%
set L_FLAGS=%LIB_PATH% %DL_FLAGS% -first init.o(Assembly$$code) %AL_FLAGS% -info totals -map %SYMBOLS% -xref -list %OUTPUT%\%PROJ%.map -elf -output %OUTPUT%\%PROJ%.axf -errors %OUTPUT%\%PROJ%.err
set P_FLAGS=%ZEROPAD% %OUTPUT%\%PROJ%.axf -bin %OUTPUT%\%PROJ%.bin
set INC=-I..\inc -I..\..\source -I..\..\arm_ucos\common\inc -I..\..\arm_ucos\ep73xx\inc


rem *************************************************************************
rem * Start build
rem *************************************************************************
md %OUTPUT%


rem *************************************************************************
rem * Compile/Assemble
rem *************************************************************************
rem * Modules having Task start code must be compiled in ARM mode.
@echo on
%ASM% ..\..\arm_ucos\ep73xx\src\init.s %A_FLAGS%
%ASM% ..\..\arm_ucos\ep73xx\src\mmu.s %A_FLAGS%
%ASM% ..\..\arm_ucos\common\src\os_cpu_a.s %A_FLAGS%
%DCC% %C_FLAGS% ..\..\arm_ucos\ep73xx\src\consol.c %INC%
%CC% %C_FLAGS% ..\..\arm_ucos\ep73xx\src\frmwrk.c %INC%
%CC% %C_FLAGS% ..\..\arm_ucos\common\src\os_cpu_c.c %INC%
%CC% %C_FLAGS% ..\..\source\ucos_ii.c %INC%
%CC% %C_FLAGS% ..\src\ex1.c %INC%
@echo off


rem *************************************************************************
rem * Link
rem *************************************************************************
@echo on
%LD% %L_FLAGS% init.o mmu.o ucos_ii.o consol.o frmwrk.o os_cpu_a.o os_cpu_c.o ex1.o
@echo off


if %TARGET_TYPE% == rom (
rem *************************************************************************
rem * FromElf - get the binary images from the elf
rem *************************************************************************
@echo on
%PLD% %P_FLAGS%
@echo off
)


rem *************************************************************************
rem * End Build
rem *************************************************************************
del *.o