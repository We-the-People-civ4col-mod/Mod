SETLOCAL
@echo off
cls
@echo parsing audio log

set audioLog=audio.log
set audioParsed=_AudioLogErrors.log

python AudioLogParser.py %audioLog% %audioParsed%

pause
ENDLOCAL