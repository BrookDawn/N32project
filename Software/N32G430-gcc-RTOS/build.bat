@echo off
chcp 65001 >nul 2>&1
setlocal enabledelayedexpansion

echo ========================================
echo  N32G430C8L7 Fast Build Script
echo ========================================

REM Get CPU core count for parallel compilation
set CORES=16
for /f "tokens=2 delims==" %%a in ('wmic cpu get NumberOfLogicalProcessors /value 2^>nul ^| find "=" 2^>nul') do set CORES=%%a
if not defined CORES (
    for /f %%a in ('echo %NUMBER_OF_PROCESSORS%') do set CORES=%%a
)
if not defined CORES set CORES=4
set /a JOBS=%CORES%
echo Detected %CORES% CPU cores, using %JOBS% parallel build jobs

REM Check ARM toolchain
echo Checking ARM toolchain...
where arm-none-eabi-gcc >nul 2>&1
if %errorlevel% NEQ 0 (
    echo [ERROR] arm-none-eabi-gcc not found!
    echo Please install GNU Arm Embedded Toolchain and add to PATH
    echo Download: https://developer.arm.com/downloads/-/gnu-rm
    echo.
    echo Installation steps:
    echo 1. Download and install GNU Arm Embedded Toolchain
    echo 2. Add installation bin folder to PATH environment variable
    echo 3. Restart command prompt and try again
    pause
    exit /b 1
)

REM Show toolchain version
for /f "tokens=*" %%i in ('arm-none-eabi-gcc --version 2^>nul ^| findstr /C:"arm-none-eabi-gcc"') do (
    echo Toolchain version: %%i
)

REM Check for clean build
if "%1"=="clean" (
    echo Cleaning build files...
    make clean
    echo Clean completed!
    pause
    exit /b 0
)

REM Build mode selection
if "%1"=="fast" (
    echo [FAST MODE] Skip dependency check, compile modified files only
    set MAKE_OPTS=-j%JOBS% --no-print-directory
) else (
    echo [STANDARD MODE] Full dependency check and compilation
    set MAKE_OPTS=-j%JOBS% --no-print-directory
)

REM Start build timing
echo.
echo Starting build...
echo Build options: %MAKE_OPTS%
echo ----------------------------------------

REM Record start time
for /f "tokens=1-3 delims=:." %%a in ("%time%") do (
    set /a start_time=%%a*3600+%%b*60+%%c
)

REM Execute parallel build
make %MAKE_OPTS%
set BUILD_RESULT=%errorlevel%

REM Calculate build time
for /f "tokens=1-3 delims=:." %%a in ("%time%") do (
    set /a end_time=%%a*3600+%%b*60+%%c
)
set /a compile_time=end_time-start_time
if %compile_time% LSS 0 set /a compile_time+=86400

echo ----------------------------------------

if %BUILD_RESULT% NEQ 0 (
    echo [FAILED] Build failed! Error code: %BUILD_RESULT%
    echo.
    echo Common solutions:
    echo 1. Check source code syntax errors
    echo 2. Verify all header file paths are correct
    echo 3. Run 'build.bat clean' to clean and rebuild
    pause
    exit /b %BUILD_RESULT%
)

echo [SUCCESS] Build completed! Time: %compile_time% seconds
echo.
echo Generated files:
if exist build\n32g430c8l7_project.elf (
    for %%F in (build\n32g430c8l7_project.elf) do echo   ELF file: %%~nxF [%%~zF bytes]
)
if exist build\n32g430c8l7_project.hex (
    for %%F in (build\n32g430c8l7_project.hex) do echo   HEX file: %%~nxF [%%~zF bytes]
)
if exist build\n32g430c8l7_project.bin (
    for %%F in (build\n32g430c8l7_project.bin) do echo   BIN file: %%~nxF [%%~zF bytes]
)
if exist build\n32g430c8l7_project.map (
    for %%F in (build\n32g430c8l7_project.map) do echo   MAP file: %%~nxF [%%~zF bytes]
)

echo.
echo Build successful! Ready for programming.
echo.
echo Usage:
echo   build.bat        - Standard build
echo   build.bat fast   - Fast incremental build
echo   build.bat clean  - Clean build files

pause
