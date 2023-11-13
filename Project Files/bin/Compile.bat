@echo off

rem adds the ability to compile any type of DLL file.
rem requires specific arguments, which can be hard to remember, through compile.pl sets them correctly.

SET PATH=%PATH%;..\..\Compiler\Microsoft Visual C++ Toolkit 2003\bin

set TARGET=%1
set TEMP_DIR_POSTFIX=%2
set CONFIG_CFLAGS= %3
bin\jom source_list /NOLOGO && ^
nmake precompile  /NOLOGO && ^
bin\jom build

pause
