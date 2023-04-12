@echo off

echo.
echo Finishing installation of We The People
echo.
echo.



if not exist Assets\CvGameCoreDLL.dll (
echo ERROR: Assets\CvGameCoreDLL.dll is missing
echo If you are using a release ^(downloaded a zip file^), then you likely ended up with the wrong zip file.
echo If you are trying to use the development version, then you failed to compile the file.
) else (

if not exist ..\..\Colonization.exe (
echo ERROR: game install dir not found
echo You need to install mods in the Mods folder inside the game install folder
echo Unlike what some people say, installing mods in My Documents will eventually result in weird bugs
) else (
rem both CvGameCoreDLL.dll and Colonization.exe found
copy "Copy to Colonization.exe"\*.dll ..\..
echo.
echo Installation complete
)
)

echo.
pause
