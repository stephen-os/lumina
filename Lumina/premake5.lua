-- Lumina Engine
-- Unified static library combining Core, Graphics, Physics, and UI modules

project "Lumina"
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
            -- Compile vertex shader to SPIR-V (shift flags must match binding_layout.cpp offsets)
            '"$(VULKAN_SDK)/Bin/dxc.exe" -T vs_6_0 -E VSMain -Fh "%{file.directory}/%{file.basename}_vs_spirv.h" -Vn g_%{file.basename}_vs_spirv -spirv -fvk-t-shift 0 0 -fvk-s-shift 32 0 -fvk-b-shift 64 0 "%{file.relpath}" -O3',
            -- Compile pixel shader to SPIR-V (shift flags must match binding_layout.cpp offsets)
            '"$(VULKAN_SDK)/Bin/dxc.exe" -T ps_6_0 -E PSMain -Fh "%{file.directory}/%{file.basename}_ps_spirv.h" -Vn g_%{file.basename}_ps_spirv -spirv -fvk-t-shift 0 0 -fvk-s-shift 32 0 -fvk-b-shift 64 0 "%{file.relpath}" -O3',
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

        -- Dependencies (in vendor folder next to src)
        "vendor/glfw/include",
        "vendor/imgui",
        "vendor/imgui/backends",
        "vendor/imgui-node-editor",
        "vendor/imgui-file-dialog",
        "vendor/glm",
        "vendor/spdlog/include",
        "vendor/nvrhi/include",
        "vendor/nvrhi/thirdparty/DirectX-Headers/include",
        "vendor/nvrhi/thirdparty/Vulkan-Headers/include",
        "vendor/stb_image",
        "vendor/stb_truetype",
        "vendor/box2d/include",
        "vendor/tracy/public",
    }

    links {
        "glfw",
        "imgui",
        "imgui-node-editor",
        "imgui-file-dialog",
        "nvrhi",
        "nvrhi-d3d12",
        "nvrhi-vk",
        "box2d",
        "tracy",
    }

    defines {
        "GLFW_INCLUDE_NONE",
        "IMGUI_DEFINE_MATH_OPERATORS",
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
            "dwmapi",
            "vulkan-1",
        }

    filter "configurations:Debug"
        defines {
            "LUMINA_DEBUG",
            "TRACY_ENABLE",
            "TRACY_ON_DEMAND",
        }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines {
            "LUMINA_RELEASE",
            "TRACY_ENABLE",
            "TRACY_ON_DEMAND",
        }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        defines { "LUMINA_DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"
