@echo off

SET PATH=%PATH%;..\..\Compiler\Microsoft Visual C++ Toolkit 2003\bin

set TARGET=Release
nmake source_list /NOLOGO && ^
nmake fastdep /NOLOGO && ^
nmake precompile  /NOLOGO && ^
bin\jom build

pause
