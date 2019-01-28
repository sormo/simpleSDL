@echo off

set batchPath=%~dp0
set convertPath=%batchPath%bin\\windows_debug\\ModelConvert.exe

set command=%convertPath% %~1
call %command%