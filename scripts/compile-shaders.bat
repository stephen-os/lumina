@echo off
REM Shader compilation script for Lumina Graphics
REM Uses DXC (DirectX Shader Compiler) to compile HLSL to DXIL and SPIR-V

setlocal enabledelayedexpansion

REM Find DXC in Windows SDK or Vulkan SDK
set DXC_PATH=
if exist "%VK_SDK_PATH%\Bin\dxc.exe" (
    set "DXC_PATH=%VK_SDK_PATH%\Bin\dxc.exe"
) else if exist "C:\VulkanSDK\1.3.296.0\Bin\dxc.exe" (
    set "DXC_PATH=C:\VulkanSDK\1.3.296.0\Bin\dxc.exe"
) else (
    REM Try to find any VulkanSDK installation
    for /d %%G in (C:\VulkanSDK\*) do (
        if exist "%%G\Bin\dxc.exe" (
            set "DXC_PATH=%%G\Bin\dxc.exe"
        )
    )
)

if "%DXC_PATH%"=="" (
    echo ERROR: Could not find dxc.exe. Please install Vulkan SDK or set VK_SDK_PATH.
    exit /b 1
)

echo Using DXC: %DXC_PATH%

set SHADER_DIR=%~dp0..\lumina\graphics\src\lumina\graphics\shaders
set OUTPUT_DIR=%SHADER_DIR%

echo Compiling shaders...

REM ============================================================================
REM Quad shader
REM ============================================================================
echo   quad.hlsl...
"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\quad_vs_dxil.h" -Vn g_quad_vs_dxil "%SHADER_DIR%\quad.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile quad vertex shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\quad_ps_dxil.h" -Vn g_quad_ps_dxil "%SHADER_DIR%\quad.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile quad pixel shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\quad_vs_spirv.h" -Vn g_quad_vs_spirv -spirv -fvk-s-shift 1 0 "%SHADER_DIR%\quad.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile quad vertex shader for SPIR-V & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\quad_ps_spirv.h" -Vn g_quad_ps_spirv -spirv -fvk-s-shift 1 0 "%SHADER_DIR%\quad.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile quad pixel shader for SPIR-V & exit /b 1 )

REM ============================================================================
REM Line shader
REM ============================================================================
echo   line.hlsl...
"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\line_vs_dxil.h" -Vn g_line_vs_dxil "%SHADER_DIR%\line.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile line vertex shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\line_ps_dxil.h" -Vn g_line_ps_dxil "%SHADER_DIR%\line.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile line pixel shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\line_vs_spirv.h" -Vn g_line_vs_spirv -spirv "%SHADER_DIR%\line.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile line vertex shader for SPIR-V & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\line_ps_spirv.h" -Vn g_line_ps_spirv -spirv "%SHADER_DIR%\line.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile line pixel shader for SPIR-V & exit /b 1 )

REM ============================================================================
REM Circle shader
REM ============================================================================
echo   circle.hlsl...
"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\circle_vs_dxil.h" -Vn g_circle_vs_dxil "%SHADER_DIR%\circle.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile circle vertex shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\circle_ps_dxil.h" -Vn g_circle_ps_dxil "%SHADER_DIR%\circle.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile circle pixel shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\circle_vs_spirv.h" -Vn g_circle_vs_spirv -spirv -fvk-s-shift 1 0 "%SHADER_DIR%\circle.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile circle vertex shader for SPIR-V & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\circle_ps_spirv.h" -Vn g_circle_ps_spirv -spirv -fvk-s-shift 1 0 "%SHADER_DIR%\circle.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile circle pixel shader for SPIR-V & exit /b 1 )

REM ============================================================================
REM Text shader
REM ============================================================================
echo   text.hlsl...
"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\text_vs_dxil.h" -Vn g_text_vs_dxil "%SHADER_DIR%\text.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile text vertex shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\text_ps_dxil.h" -Vn g_text_ps_dxil "%SHADER_DIR%\text.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile text pixel shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\text_vs_spirv.h" -Vn g_text_vs_spirv -spirv -fvk-s-shift 1 0 "%SHADER_DIR%\text.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile text vertex shader for SPIR-V & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\text_ps_spirv.h" -Vn g_text_ps_spirv -spirv -fvk-s-shift 1 0 "%SHADER_DIR%\text.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile text pixel shader for SPIR-V & exit /b 1 )

REM ============================================================================
REM Triangle shader
REM ============================================================================
echo   triangle.hlsl...
"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\triangle_vs_dxil.h" -Vn g_triangle_vs_dxil "%SHADER_DIR%\triangle.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile triangle vertex shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\triangle_ps_dxil.h" -Vn g_triangle_ps_dxil "%SHADER_DIR%\triangle.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile triangle pixel shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\triangle_vs_spirv.h" -Vn g_triangle_vs_spirv -spirv -fvk-s-shift 1 0 "%SHADER_DIR%\triangle.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile triangle vertex shader for SPIR-V & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\triangle_ps_spirv.h" -Vn g_triangle_ps_spirv -spirv -fvk-s-shift 1 0 "%SHADER_DIR%\triangle.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile triangle pixel shader for SPIR-V & exit /b 1 )

REM ============================================================================
REM Pixel shader
REM ============================================================================
echo   pixel.hlsl...
"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\pixel_vs_dxil.h" -Vn g_pixel_vs_dxil "%SHADER_DIR%\pixel.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile pixel vertex shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\pixel_ps_dxil.h" -Vn g_pixel_ps_dxil "%SHADER_DIR%\pixel.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile pixel pixel shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\pixel_vs_spirv.h" -Vn g_pixel_vs_spirv -spirv "%SHADER_DIR%\pixel.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile pixel vertex shader for SPIR-V & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\pixel_ps_spirv.h" -Vn g_pixel_ps_spirv -spirv "%SHADER_DIR%\pixel.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile pixel pixel shader for SPIR-V & exit /b 1 )

REM ============================================================================
REM Grid shader
REM ============================================================================
echo   grid.hlsl...
"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\grid_vs_dxil.h" -Vn g_grid_vs_dxil "%SHADER_DIR%\grid.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile grid vertex shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\grid_ps_dxil.h" -Vn g_grid_ps_dxil "%SHADER_DIR%\grid.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile grid pixel shader for DXIL & exit /b 1 )

"%DXC_PATH%" -T vs_6_0 -E VSMain -Fh "%OUTPUT_DIR%\grid_vs_spirv.h" -Vn g_grid_vs_spirv -spirv "%SHADER_DIR%\grid.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile grid vertex shader for SPIR-V & exit /b 1 )

"%DXC_PATH%" -T ps_6_0 -E PSMain -Fh "%OUTPUT_DIR%\grid_ps_spirv.h" -Vn g_grid_ps_spirv -spirv "%SHADER_DIR%\grid.hlsl" -O3
if errorlevel 1 ( echo ERROR: Failed to compile grid pixel shader for SPIR-V & exit /b 1 )

REM ============================================================================
REM Create combined header files
REM ============================================================================
echo Creating combined headers...

REM Quad combined
echo // Auto-generated combined quad shader header > "%OUTPUT_DIR%\quad_vs.h"
echo #pragma once >> "%OUTPUT_DIR%\quad_vs.h"
echo #include "quad_vs_dxil.h" >> "%OUTPUT_DIR%\quad_vs.h"
echo #include "quad_vs_spirv.h" >> "%OUTPUT_DIR%\quad_vs.h"

echo // Auto-generated combined quad shader header > "%OUTPUT_DIR%\quad_ps.h"
echo #pragma once >> "%OUTPUT_DIR%\quad_ps.h"
echo #include "quad_ps_dxil.h" >> "%OUTPUT_DIR%\quad_ps.h"
echo #include "quad_ps_spirv.h" >> "%OUTPUT_DIR%\quad_ps.h"

REM Line combined
echo // Auto-generated combined line shader header > "%OUTPUT_DIR%\line_vs.h"
echo #pragma once >> "%OUTPUT_DIR%\line_vs.h"
echo #include "line_vs_dxil.h" >> "%OUTPUT_DIR%\line_vs.h"
echo #include "line_vs_spirv.h" >> "%OUTPUT_DIR%\line_vs.h"

echo // Auto-generated combined line shader header > "%OUTPUT_DIR%\line_ps.h"
echo #pragma once >> "%OUTPUT_DIR%\line_ps.h"
echo #include "line_ps_dxil.h" >> "%OUTPUT_DIR%\line_ps.h"
echo #include "line_ps_spirv.h" >> "%OUTPUT_DIR%\line_ps.h"

REM Circle combined
echo // Auto-generated combined circle shader header > "%OUTPUT_DIR%\circle_vs.h"
echo #pragma once >> "%OUTPUT_DIR%\circle_vs.h"
echo #include "circle_vs_dxil.h" >> "%OUTPUT_DIR%\circle_vs.h"
echo #include "circle_vs_spirv.h" >> "%OUTPUT_DIR%\circle_vs.h"

echo // Auto-generated combined circle shader header > "%OUTPUT_DIR%\circle_ps.h"
echo #pragma once >> "%OUTPUT_DIR%\circle_ps.h"
echo #include "circle_ps_dxil.h" >> "%OUTPUT_DIR%\circle_ps.h"
echo #include "circle_ps_spirv.h" >> "%OUTPUT_DIR%\circle_ps.h"

REM Text combined
echo // Auto-generated combined text shader header > "%OUTPUT_DIR%\text_vs.h"
echo #pragma once >> "%OUTPUT_DIR%\text_vs.h"
echo #include "text_vs_dxil.h" >> "%OUTPUT_DIR%\text_vs.h"
echo #include "text_vs_spirv.h" >> "%OUTPUT_DIR%\text_vs.h"

echo // Auto-generated combined text shader header > "%OUTPUT_DIR%\text_ps.h"
echo #pragma once >> "%OUTPUT_DIR%\text_ps.h"
echo #include "text_ps_dxil.h" >> "%OUTPUT_DIR%\text_ps.h"
echo #include "text_ps_spirv.h" >> "%OUTPUT_DIR%\text_ps.h"

REM Triangle combined
echo // Auto-generated combined triangle shader header > "%OUTPUT_DIR%\triangle_vs.h"
echo #pragma once >> "%OUTPUT_DIR%\triangle_vs.h"
echo #include "triangle_vs_dxil.h" >> "%OUTPUT_DIR%\triangle_vs.h"
echo #include "triangle_vs_spirv.h" >> "%OUTPUT_DIR%\triangle_vs.h"

echo // Auto-generated combined triangle shader header > "%OUTPUT_DIR%\triangle_ps.h"
echo #pragma once >> "%OUTPUT_DIR%\triangle_ps.h"
echo #include "triangle_ps_dxil.h" >> "%OUTPUT_DIR%\triangle_ps.h"
echo #include "triangle_ps_spirv.h" >> "%OUTPUT_DIR%\triangle_ps.h"

REM Pixel combined
echo // Auto-generated combined pixel shader header > "%OUTPUT_DIR%\pixel_vs.h"
echo #pragma once >> "%OUTPUT_DIR%\pixel_vs.h"
echo #include "pixel_vs_dxil.h" >> "%OUTPUT_DIR%\pixel_vs.h"
echo #include "pixel_vs_spirv.h" >> "%OUTPUT_DIR%\pixel_vs.h"

echo // Auto-generated combined pixel shader header > "%OUTPUT_DIR%\pixel_ps.h"
echo #pragma once >> "%OUTPUT_DIR%\pixel_ps.h"
echo #include "pixel_ps_dxil.h" >> "%OUTPUT_DIR%\pixel_ps.h"
echo #include "pixel_ps_spirv.h" >> "%OUTPUT_DIR%\pixel_ps.h"

REM Grid combined
echo // Auto-generated combined grid shader header > "%OUTPUT_DIR%\grid_vs.h"
echo #pragma once >> "%OUTPUT_DIR%\grid_vs.h"
echo #include "grid_vs_dxil.h" >> "%OUTPUT_DIR%\grid_vs.h"
echo #include "grid_vs_spirv.h" >> "%OUTPUT_DIR%\grid_vs.h"

echo // Auto-generated combined grid shader header > "%OUTPUT_DIR%\grid_ps.h"
echo #pragma once >> "%OUTPUT_DIR%\grid_ps.h"
echo #include "grid_ps_dxil.h" >> "%OUTPUT_DIR%\grid_ps.h"
echo #include "grid_ps_spirv.h" >> "%OUTPUT_DIR%\grid_ps.h"

echo Shader compilation complete!
