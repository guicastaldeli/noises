@echo off
setlocal EnableDelayedExpansion

set "ROOT_DIR=%~dp0.."
set "BUILD_DIR=%ROOT_DIR%\.build"
set "OUT_EXE=%BUILD_DIR%\perlins.exe"
set "GLFW_INCLUDE=C:\Users\casta\OneDrive\Desktop\glfw-3.4\include"
set "GLFW_LIB_DIR=C:\Users\casta\OneDrive\Desktop\glfw-3.4\src"

set "VK_VALID=0"
if defined VULKAN_SDK (
    if exist "%VULKAN_SDK%\Include\vulkan\vulkan.h" (
        if exist "%VULKAN_SDK%\Lib\vulkan-1.lib" (
            set "VK_VALID=1"
        )
    )
)

if "%VK_VALID%"=="0" (
    echo [build] VULKAN_SDK not set or invalid, probing C:\VulkanSDK\ ...
    set "VULKAN_SDK="
    for /f "delims=" %%d in ('dir /b /ad /o-n "C:\VulkanSDK" 2^>nul') do (
        if not defined VULKAN_SDK (
            set "CAND=C:\VulkanSDK\%%d"
            if exist "!CAND!\Include\vulkan\vulkan.h" (
                if exist "!CAND!\Lib\vulkan-1.lib" (
                    set "VULKAN_SDK=!CAND!"
                )
            )
        )
    )
)

if not defined VULKAN_SDK (
    echo ERROR: Could not locate Vulkan SDK.
    echo   - Set VULKAN_SDK to your SDK root, e.g.
    echo       setx VULKAN_SDK "C:\VulkanSDK\1.4.363.0"
    echo     then CLOSE and REOPEN this terminal.
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

echo [build] Using VULKAN_SDK = %VULKAN_SDK%

if not exist "%VULKAN_SDK%\Include\vulkan\vulkan.h" (
    echo ERROR: Missing "%VULKAN_SDK%\Include\vulkan\vulkan.h"
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)
if not exist "%VULKAN_SDK%\Lib\vulkan-1.lib" (
    echo ERROR: Missing "%VULKAN_SDK%\Lib\vulkan-1.lib"
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

if not exist "%SystemRoot%\System32\vulkan-1.dll" (
    echo [build] WARNING: %SystemRoot%\System32\vulkan-1.dll not found.
    echo [build]          Update your GPU driver if the exe fails to start.
)

cd /d "%ROOT_DIR%" || (
    echo ERROR: Could not cd into "%ROOT_DIR%"
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

where g++ >nul 2>nul
if errorlevel 1 (
    echo ERROR: g++ not found on PATH.
    echo Add your MinGW bin folder to PATH and try again.
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

echo Building perlins...
echo =====================================================
echo   ROOT_DIR   = %ROOT_DIR%
echo   BUILD_DIR  = %BUILD_DIR%
echo   VULKAN_SDK = %VULKAN_SDK%
echo =====================================================

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo.
echo Cleaning previous builds...
del /q "%BUILD_DIR%\*.exe" 2>nul

echo.
echo Collecting .cpp files...
set "CPP_LIST="
for /r "%ROOT_DIR%" %%f in (*.cpp) do (
    echo %%f | findstr /i /c:"\.build\\" >nul
    if errorlevel 1 (
        echo   %%f
        set "CPP_LIST=!CPP_LIST! "%%f""
    )
)

if "!CPP_LIST!"=="" (
    echo ERROR: No .cpp files found.
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

echo.
echo Compiling...
g++ -std=c++17 -O2 -g ^
    -static-libgcc -static-libstdc++ ^
    -I"%ROOT_DIR%" ^
    -I"%ROOT_DIR%\src" ^
    -I"%VULKAN_SDK%\Include" ^
    -I"%GLFW_INCLUDE%" ^
    -L"%VULKAN_SDK%\Lib" ^
    -L"%GLFW_LIB_DIR%" ^
    -o "%OUT_EXE%" ^
    !CPP_LIST! ^
    "%VULKAN_SDK%\Lib\vulkan-1.lib" ^
    -lglfw3 -lgdi32 -luser32 -lshell32 -lws2_32

if errorlevel 1 (
    echo.
    echo ERROR: Build failed.
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

echo.
if not exist "%OUT_EXE%" (
    echo ERROR: Output not created.
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

echo.
echo BUILD SUCCESSFUL!
echo   Exe : %OUT_EXE%
echo.
echo Run it with:
echo   "%OUT_EXE%"
echo.
if not defined BUILD_FROM_RUN pause
endlocal