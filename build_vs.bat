@echo off

if not exist toolchain\vcpkg\vcpkg.exe (
    echo Bootstrapping vcpkg...
    call toolchain\vcpkg\bootstrap-vcpkg.bat -disableMetrics
    if %errorlevel% neq 0 (
        echo vcpkg bootstrap failed!
        exit /b %errorlevel%
    )
)

cmake --preset vs2026
if %errorlevel% neq 0 (
    echo CMake generation failed!
    exit /b %errorlevel%
)

cmake --build --preset debug
if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
)

echo Build successful!
