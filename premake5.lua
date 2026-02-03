-- Lumina Meta-Repository Premake5
-- Main build configuration

workspace "Lumina"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }
    startproject "sandbox"

    -- Output directory pattern
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- Workspace-wide settings
    flags { "MultiProcessorCompile" }

-- Dependencies (external libraries)
group "Dependencies"
    include "premake/dep-configs/glfw"
    include "premake/dep-configs/imgui"
    include "premake/dep-configs/nvrhi"
    -- spdlog is header-only, no build needed
group ""

-- Lumina Modules
group "Lumina"
    include "lumina/core"
    include "lumina/ui"
    include "lumina/graphics"
group ""

-- Sandbox/Test Application
group "Sandbox"
    project "sandbox"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++23"
        staticruntime "off"

        targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
        objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

        files {
            "sandbox/**.h",
            "sandbox/**.cpp"
        }

        includedirs {
            "lumina/core/src",
            "lumina/ui/src",
            "lumina/graphics/src",
            "dependencies/glfw/include",
            "dependencies/glm",
            "dependencies/imgui",
            "dependencies/imgui/backends",
            "dependencies/spdlog/include",
            "dependencies/nvrhi/include",
            "dependencies/nvrhi/thirdparty/DirectX-Headers/include",
            "dependencies/nvrhi/thirdparty/Vulkan-Headers/include",
        }

        links {
            "core",
            "ui",
            "graphics",
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

        buildoptions { "/utf-8" }

        filter "system:windows"
            systemversion "latest"
            defines {
                "LUMINA_PLATFORM_WINDOWS",
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
group ""
