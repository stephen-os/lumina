-- core premake5.lua
-- This file defines the core static library

project "core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++23"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "src",

        -- Dependencies (paths relative to workspace location)
        "%{wks.location}/dependencies/glfw/include",
        "%{wks.location}/dependencies/imgui",
        "%{wks.location}/dependencies/imgui/backends",
        "%{wks.location}/dependencies/spdlog/include",
        "%{wks.location}/dependencies/nvrhi/include",
        "%{wks.location}/dependencies/nvrhi/thirdparty/DirectX-Headers/include",
        "%{wks.location}/dependencies/nvrhi/thirdparty/Vulkan-Headers/include",
    }

    links {
        "glfw",
        "imgui",
        "nvrhi",
        "nvrhi-d3d12",
        "nvrhi-vk",
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
