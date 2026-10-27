@echo off
setlocal EnableDelayedExpansion

set "ROOT_DIR=%~dp0.."
set "BUILD_DIR=%ROOT_DIR%\.build"
set "OUT_EXE=%BUILD_DIR%\noises.exe"
set "GLFW_INCLUDE=C:\Users\casta\OneDrive\Desktop\glfw-3.4\include"
set "GLFW_LIB_DIR=C:\Users\casta\OneDrive\Desktop\glfw-3.4\src"

set "LIB_SRC_DIR=%ROOT_DIR%\.lib"
set "LIB_OBJ_DIR=%BUILD_DIR%\obj"
set "IMGUI_DIR=%LIB_SRC_DIR%\imgui-1.93.0"

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
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

echo [build] Using VULKAN_SDK = %VULKAN_SDK%

if not exist "%VULKAN_SDK%\Include\vulkan\vulkan.h" (
    echo ERROR: Missing Vulkan SDK headers.
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)
if not exist "%VULKAN_SDK%\Lib\vulkan-1.lib" (
    echo ERROR: Missing vulkan-1.lib.
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)
if not exist "%SystemRoot%\System32\vulkan-1.dll" (
    echo [build] WARNING: vulkan-1.dll not in System32.
)

if not exist "%LIB_SRC_DIR%" (
    echo [build] WARNING: "%LIB_SRC_DIR%" does not exist.
)

cd /d "%ROOT_DIR%" || (
    echo ERROR: Could not cd into "%ROOT_DIR%"
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

where g++ >nul 2>nul
if not %errorlevel% == 0 (
    echo ERROR: g++ not found on PATH.
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

echo Building noises...
echo =====================================================
echo   ROOT_DIR   = %ROOT_DIR%
echo   BUILD_DIR  = %BUILD_DIR%
echo   LIB_SRC    = %LIB_SRC_DIR%
echo   VULKAN_SDK = %VULKAN_SDK%
echo =====================================================

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo.
echo Cleaning previous builds...
del /q "%BUILD_DIR%\*.exe" 2>nul

echo.
echo Compiling shaders...
set "SHADERS_FROM_BUILD=1"
call "%ROOT_DIR%\shaders\compile.bat"
if not %errorlevel% == 0 (
    echo ERROR: Shader compilation failed.
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

echo.
echo =====================================================
echo   Collecting .cpp files...
echo =====================================================

set "CPP_LIST="
set "CPP_COUNT=0"
for /r "%ROOT_DIR%" %%f in (*.cpp) do (
    echo %%f | findstr /i /c:"\.build\\" >nul
    if errorlevel 1 (
        echo %%f | findstr /i /c:"\.lib\\" >nul
        if errorlevel 1 (
            echo   [file] %%f
            set "CPP_LIST=!CPP_LIST! "%%f""
            set /a CPP_COUNT+=1
        )
    )
)

if !CPP_COUNT! EQU 0 (
    echo ERROR: No .cpp files found.
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

echo.
echo =====================================================
echo   Collecting library .cpp files...
echo =====================================================

if not exist "%LIB_OBJ_DIR%" mkdir "%LIB_OBJ_DIR%"

set "LIB_CPP_LIST="
set "LIB_CPP_COUNT=0"
for %%f in (
    "%IMGUI_DIR%\imgui.cpp"
    "%IMGUI_DIR%\imgui_draw.cpp"
    "%IMGUI_DIR%\imgui_tables.cpp"
    "%IMGUI_DIR%\imgui_widgets.cpp"
    "%IMGUI_DIR%\backends\imgui_impl_glfw.cpp"
    "%IMGUI_DIR%\backends\imgui_impl_vulkan.cpp"
) do (
    if exist "%%~f" (
        echo   [lib]  %%~f
        set "LIB_CPP_LIST=!LIB_CPP_LIST! "%%~f""
        set /a LIB_CPP_COUNT+=1
    ) else (
        echo   [lib]  MISSING  %%~f
    )
)

echo [diag] LIB_CPP_COUNT=!LIB_CPP_COUNT!

if !LIB_CPP_COUNT! EQU 0 (
    echo [build] no library sources found
    echo [build] check IMGUI_DIR = %IMGUI_DIR%
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

echo.
echo =====================================================
echo   Compiling libraries...
echo =====================================================

for %%f in (!LIB_CPP_LIST!) do (
    set "OBJ=%LIB_OBJ_DIR%\%%~nf.o"

    if not exist "!OBJ!" (
        echo   [cc] %%~f
        g++ -std=c++17 -O2 -g -c "%%~f" -o "!OBJ!" ^
            -I"%ROOT_DIR%" ^
            -I"%ROOT_DIR%\src" ^
            -I"%VULKAN_SDK%\Include" ^
            -I"%GLFW_INCLUDE%" ^
            -I"%IMGUI_DIR%" ^
            -I"%IMGUI_DIR%\backends"
        if not !errorlevel! == 0 (
            echo ERROR: Failed to compile %%~f
            if not defined BUILD_FROM_RUN pause
            exit /b 1
        )
    ) else (
        echo   [ok] %%~f
    )
)

set "LIB_OBJ_LIST="
set "LIB_OBJ_COUNT=0"
for %%o in ("%LIB_OBJ_DIR%\*.o") do (
    set "LIB_OBJ_LIST=!LIB_OBJ_LIST! "%%~fo""
    set /a LIB_OBJ_COUNT+=1
)

echo [diag] LIB_OBJ_COUNT=!LIB_OBJ_COUNT!

if !LIB_OBJ_COUNT! EQU 0 (
    echo ERROR: No library object files found in "%LIB_OBJ_DIR%"
    if not defined BUILD_FROM_RUN pause
    exit /b 1
)

echo.
echo Library objects ^(%LIB_OBJ_COUNT%^):
for %%o in (!LIB_OBJ_LIST!) do echo   %%~o

echo.
echo =====================================================
echo   Compiling...
echo =====================================================

g++ -std=c++17 -O2 -g ^
    -static-libgcc -static-libstdc++ ^
    -I"%ROOT_DIR%" ^
    -I"%ROOT_DIR%\src" ^
    -I"%VULKAN_SDK%\Include" ^
    -I"%GLFW_INCLUDE%" ^
    -I"%IMGUI_DIR%" ^
    -I"%IMGUI_DIR%\backends" ^
    -L"%VULKAN_SDK%\Lib" ^
    -L"%GLFW_LIB_DIR%" ^
    -o "%OUT_EXE%" ^
    !CPP_LIST! ^
    !LIB_OBJ_LIST! ^
    "%VULKAN_SDK%\Lib\vulkan-1.lib" ^
    -lglfw3 -lgdi32 -luser32 -lshell32 -lws2_32

if not !errorlevel! == 0 (
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
if not defined BUILD_FROM_RUN pause
endlocal