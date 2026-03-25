@echo off
setlocal

echo ========================================
echo Lumina Build Setup
echo ========================================
echo.

:: Check if premake5 exists in PATH or locally
where premake5 >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo Found premake5 in PATH
    set PREMAKE=premake5
) else if exist "%~dp0..\dependencies\premake\premake5.exe" (
    echo Found premake5 in dependencies folder
    set PREMAKE=%~dp0..\dependencies\premake\premake5.exe
) else (
    echo ERROR: premake5 not found!
    echo Please install premake5 and add it to your PATH
    echo Or download it to: dependencies\premake\premake5.exe
    echo.
    echo Download from: https://premake.github.io/download
    pause
    exit /b 1
)

:: Generate Visual Studio 2022 solution
echo Generating Visual Studio 2022 solution...
pushd %~dp0..
%PREMAKE% vs2022
popd

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Build files generated successfully!
    echo Open Lumina.sln in Visual Studio
    echo ========================================
) else (
    echo.
    echo ERROR: Failed to generate build files
)

pause
