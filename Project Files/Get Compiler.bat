@echo off

SET PATH=%PATH%;"%ProgramFiles(x86)%\Git";"%ProgramFiles%\Git";"%ProgramFiles(x86)%\SmartGit\git\bin";"%ProgramFiles%\SmartGit\git\bin";

if exist ..\..\Compiler (
    cd ..\..\Compiler
	git pull --rebase
) else (
	git clone git@github.com:We-the-People-civ4col-mod/Compiler.git ..\..\Compiler
    cd ..\..\Compiler
)

if not exist ..\..\tbb.dll (
	echo Copying tbb.dll to Colonization.exe
	copy DLL_for_exe\tbb.dll ..\..\
)
if not exist ..\..\tbbmalloc.dll (
	echo Copying tbbmalloc.dll to Colonization.exe
	copy DLL_for_exe\tbbmalloc.dll ..\..\
)

if not exist "C:\Strawberry" (
	echo No pearl found
	echo Perl is required for the compiler to work properly.
	echo Please install the 64 bit version of Strawberry perl. Hit any key to open the download URL.
	echo The compiler should be fully installed and working once perl is installed.
	pause
	start "" https://strawberryperl.com/
)

pause
