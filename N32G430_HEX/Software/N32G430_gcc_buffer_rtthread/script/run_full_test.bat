@echo off
REM =====================================================
REM N32G430 RT-Thread Full Test Script
REM Build, Flash, and Run UART Test
REM =====================================================

echo =====================================================
echo N32G430 RT-Thread Nano - Full Test
echo =====================================================
echo.

REM Set COM port (modify this to match your setup)
set COM_PORT=COM3
set BAUDRATE=115200

REM Change to GCC directory
cd /d "%~dp0..\GCC"

REM Clean and build
echo [1/4] Cleaning previous build...
if exist build rmdir /s /q build
echo Done.
echo.

echo [2/4] Building project...
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
echo [3/4] Flashing program to device...
make download
if errorlevel 1 (
    echo.
    echo [ERROR] Flash failed!
    echo Please check:
    echo   1. J-Link is connected
    echo   2. Target board is powered
    echo   3. J-Link software is installed
    pause
    exit /b 1
)
echo.
echo [OK] Flash successful!
echo.

REM Wait for device to boot
echo [4/4] Waiting for device to boot...
timeout /t 3 /nobreak > nul
echo.

REM Change to script directory
cd /d "%~dp0"

REM Run UART test
echo =====================================================
echo Running UART1 Test...
echo =====================================================
echo.

REM Check if Python is available
where python >nul 2>nul
if errorlevel 1 (
    echo [ERROR] Python not found in PATH
    echo Please install Python and add it to PATH
    pause
    exit /b 1
)

REM Check if pyserial is installed
python -c "import serial" 2>nul
if errorlevel 1 (
    echo [INFO] Installing pyserial...
    pip install pyserial
    if errorlevel 1 (
        echo [ERROR] Failed to install pyserial
        pause
        exit /b 1
    )
)

REM Run the test script
python test_uart1.py %COM_PORT% %BAUDRATE%

echo.
echo =====================================================
echo Test Complete!
echo =====================================================
echo.

pause
