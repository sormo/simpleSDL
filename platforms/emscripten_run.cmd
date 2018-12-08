@echo off
setlocal
set PROJECT_DIR=%~dp0..
set BIN_DIR=%PROJECT_DIR%\bin\emscripten

call "c:\src\emsdk\emsdk_env.bat"

call c:\src\emscripten\emrun --no_emrun_detect "%BIN_DIR%\SimpleExample.html" || goto error
