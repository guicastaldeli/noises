@echo off
setlocal

set ROOT_DIR=%~dp0
set BUILD_DIR=%ROOT_DIR%.build
set "BUILD_FROM_RUN=1"

echo [run] Building...
call "%BUILD_DIR%\build.bat"
if not %errorlevel% == 0 (
    echo ERROR: Build failed, aborting run.
    pause
    exit /b 1
)

cd /d "%BUILD_DIR%" || (
    echo ERROR: Build folder not found.
    pause
    exit /b 1
)

if not exist noises.exe (
    echo ERROR: noises.exe not found after build.
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