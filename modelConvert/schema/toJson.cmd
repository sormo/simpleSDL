@echo off

set filePath=%~1
set fileDir=%~dp1
set batchPath=%~dp0

set flatcPath=%batchPath%flatc.exe
set schemaPath=%batchPath%model.fbs

set command=%flatcPath% --json --raw-binary %schemaPath% -- %~1
call %command%
