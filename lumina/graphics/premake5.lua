-- graphics premake5.lua
-- This file defines the graphics static library

project "graphics"
    kind "StaticLib"
    language "C++"
    cppdialect "C++23"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**.h",
        "src/**.cpp",
        "src/**.hlsl"
    }

    -- Custom build rules for HLSL shaders
    filter "files:**.hlsl"
        buildmessage "Compiling shader %{file.name}..."
        buildcommands {
            -- Compile vertex shader to DXIL
            '"$(VULKAN_SDK)/Bin/dxc.exe" -T vs_6_0 -E VSMain -Fh "%{file.directory}/%{file.basename}_vs_dxil.h" -Vn g_%{file.basename}_vs_dxil "%{file.relpath}" -O3',
            -- Compile pixel shader to DXIL
            '"$(VULKAN_SDK)/Bin/dxc.exe" -T ps_6_0 -E PSMain -Fh "%{file.directory}/%{file.basename}_ps_dxil.h" -Vn g_%{file.basename}_ps_dxil "%{file.relpath}" -O3',
            -- Compile vertex shader to SPIR-V
            '"$(VULKAN_SDK)/Bin/dxc.exe" -T vs_6_0 -E VSMain -Fh "%{file.directory}/%{file.basename}_vs_spirv.h" -Vn g_%{file.basename}_vs_spirv -spirv "%{file.relpath}" -O3',
            -- Compile pixel shader to SPIR-V
            '"$(VULKAN_SDK)/Bin/dxc.exe" -T ps_6_0 -E PSMain -Fh "%{file.directory}/%{file.basename}_ps_spirv.h" -Vn g_%{file.basename}_ps_spirv -spirv "%{file.relpath}" -O3',
            -- Create combined vertex shader header
            'echo // Auto-generated combined %{file.basename} vertex shader header > "%{file.directory}/%{file.basename}_vs.h"',
            'echo #pragma once >> "%{file.directory}/%{file.basename}_vs.h"',
            'echo #include "%{file.basename}_vs_dxil.h" >> "%{file.directory}/%{file.basename}_vs.h"',
            'echo #include "%{file.basename}_vs_spirv.h" >> "%{file.directory}/%{file.basename}_vs.h"',
            -- Create combined pixel shader header
            'echo // Auto-generated combined %{file.basename} pixel shader header > "%{file.directory}/%{file.basename}_ps.h"',
            'echo #pragma once >> "%{file.directory}/%{file.basename}_ps.h"',
            'echo #include "%{file.basename}_ps_dxil.h" >> "%{file.directory}/%{file.basename}_ps.h"',
            'echo #include "%{file.basename}_ps_spirv.h" >> "%{file.directory}/%{file.basename}_ps.h"',
        }
        buildoutputs {
            "%{file.directory}/%{file.basename}_vs_dxil.h",
            "%{file.directory}/%{file.basename}_ps_dxil.h",
            "%{file.directory}/%{file.basename}_vs_spirv.h",
            "%{file.directory}/%{file.basename}_ps_spirv.h",
            "%{file.directory}/%{file.basename}_vs.h",
            "%{file.directory}/%{file.basename}_ps.h",
        }
    filter {}

    includedirs {
        "src",

        -- Lumina modules
        "%{wks.location}/lumina/core/src",

        -- Dependencies
        "%{wks.location}/dependencies/glfw/include",
        "%{wks.location}/dependencies/glm",
        "%{wks.location}/dependencies/spdlog/include",
        "%{wks.location}/dependencies/nvrhi/include",
        "%{wks.location}/dependencies/nvrhi/thirdparty/DirectX-Headers/include",
        "%{wks.location}/dependencies/nvrhi/thirdparty/Vulkan-Headers/include",
        "%{wks.location}/dependencies/stb_truetype",
    }

    links {
        "core",
        "glfw",
        "nvrhi",
        "nvrhi-d3d12",
        "nvrhi-vk",
    }

    defines {
        "GLFW_INCLUDE_NONE",
        "NVRHI_WITH_DX12=1",
        "NVRHI_WITH_VULKAN=1",
        "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1",
    }

    flags { "MultiProcessorCompile" }
    buildoptions { "/utf-8" }

    filter "system:windows"
        systemversion "latest"
        defines {
            "LUMINA_PLATFORM_WINDOWS",
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX",
            "VK_USE_PLATFORM_WIN32_KHR",
        }
        libdirs { "$(VULKAN_SDK)/Lib" }
        links {
            "d3d12",
            "dxgi",
            "dxguid",
            "vulkan-1",
        }

    filter "configurations:Debug"
        defines { "LUMINA_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "LUMINA_RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        defines { "LUMINA_DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"
