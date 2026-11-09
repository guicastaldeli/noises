@echo off
setlocal EnableDelayedExpansion

set "GLSLC="
if defined VULKAN_SDK (
    if exist "%VULKAN_SDK%\Bin\glslc.exe" set "GLSLC=%VULKAN_SDK%\Bin\glslc.exe"
)
if not defined GLSLC (
    if exist "C:\VulkanSDK\1.4.363.0\Bin\glslc.exe" (
        set "GLSLC=C:\VulkanSDK\1.4.363.0\Bin\glslc.exe"
    )
)
if not defined GLSLC (
    echo ERROR: glslc.exe not found. Set VULKAN_SDK or install the Vulkan SDK.
    if not defined SHADERS_FROM_BUILD pause
    exit /b 1
)

set "SRC_DIR=%~dp0"
set "OUT_DIR=%BUILD_DIR%\"
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo [shaders] glslc  = %GLSLC%
echo [shaders] source = %SRC_DIR%
echo [shaders] output = %OUT_DIR%

set "FAIL=0"
for %%P in (
    "shader.vert=vert"
    "shader.frag=frag"
    "shadow.vert=shadow"
) do (
    for /f "tokens=1,2 delims==" %%A in (%%P) do (
        set "SRC=%%~A"
        set "STAGE=%%~B"
        set "OUT=%OUT_DIR%!STAGE!.spv"
        echo [shaders] !SRC! -^> !OUT!
        "%GLSLC%" "%SRC_DIR%!SRC!" -o "!OUT!" --target-env=vulkan1.2 -I "%SRC_DIR%"
        if errorlevel 1 set "FAIL=1"
    )
)

if "%FAIL%"=="1" (
    echo ERROR: One or more shaders failed to compile.
    if not defined SHADERS_FROM_BUILD pause
    exit /b 1
)

echo [shaders] OK
if not defined SHADERS_FROM_BUILD pause
endlocal