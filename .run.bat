@echo off
setlocal

set ROOT_DIR=%~dp0
set BUILD_DIR=%ROOT_DIR%\.build

cd /d "%BUILD_DIR%" || (
    echo ERROR: Build folder not found.
    pause
    exit /b 1
)

if not exist perlins.exe (
    echo ERROR: perlins.exe not found. Run build.bat first.
    pause
    exit /b 1
)

echo Running...
perlins.exe %*
echo.
echo Exited with code %errorlevel%.
pause
endlocal