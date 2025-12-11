@echo off
REM =====================================================
REM N32G430 RT-Thread Build and Flash Script
REM =====================================================

echo =====================================================
echo N32G430 RT-Thread Nano - Build and Flash
echo =====================================================
echo.

REM Change to GCC directory
cd /d "%~dp0..\GCC"

REM Clean previous build (optional)
echo [1/3] Cleaning previous build...
if exist build rmdir /s /q build
echo Done.
echo.

REM Build the project
echo [2/3] Building project...
make all
if errorlevel 1 (
    echo.
    echo [ERROR] Build failed!
    pause
    exit /b 1
)
echo.
echo [OK] Build successful!
echo.

REM Flash the program
echo [3/3] Flashing program to device...
make download
if errorlevel 1 (
    echo.
    echo [ERROR] Flash failed!
    pause
    exit /b 1
)

echo.
echo =====================================================
echo Build and Flash Complete!
echo =====================================================
echo.

pause
