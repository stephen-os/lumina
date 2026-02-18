@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Applying Patches
echo ========================================
echo.

pushd %~dp0..
set REPO_DIR=%CD%
popd

set PATCHES_DIR=%REPO_DIR%\patches
set TARGET_DIR=%REPO_DIR%\dependencies\imgui

set PASS=0
set SKIP=0
set FAIL=0

for %%f in ("%PATCHES_DIR%\*.patch") do (
    echo %%~nxf

    git -C "%TARGET_DIR%" apply --reverse --check "%%f" >nul 2>&1
    if !ERRORLEVEL! EQU 0 (
        echo   Skipped: already applied
        set /a SKIP+=1
    ) else (
        git -C "%TARGET_DIR%" apply "%%f" >nul 2>&1
        if !ERRORLEVEL! EQU 0 (
            echo   Applied
            set /a PASS+=1
        ) else (
            echo   FAILED: patch does not apply cleanly
            set /a FAIL+=1
        )
    )
)

echo.
echo ========================================
echo %PASS% applied, %SKIP% skipped, %FAIL% failed
echo ========================================

if %FAIL% GTR 0 (
    echo.
    echo WARNING: Some patches failed. Check for conflicts.
)

endlocal
pause
