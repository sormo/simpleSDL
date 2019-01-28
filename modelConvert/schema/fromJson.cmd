@echo off

set file
set filePath=%~1
set fileDir=%~dp1
set batchPath=%~dp0

set flatcPath=%batchPath%flatc.exe
set schemaPath=%batchPath%model.fbs

set command=%flatcPath% --binary %schemaPath% %~1
call %command%

set fileName=%~n1
move /y %fileName%.bin %fileName%.model