@echo off
setlocal

set ROOT_DIR=%~dp0
set BUILD_DIR=%ROOT_DIR%.build

cd /d "%BUILD_DIR%" || (
    echo ERROR: Build folder not found.
    pause
    exit /b 1
)

if not exist noises.exe (
    echo ERROR: noises.exe not found.
    echo Run .build\build.bat first to build it.
    pause
    exit /b 1
)

echo.
echo Running...
noises.exe %*
set "EXITCODE=%errorlevel%"
echo.
echo Exited with code %EXITCODE%.
pause
endlocal