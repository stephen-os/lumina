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
group "dependencies"
    include "dependencies/config/glfw"
    include "dependencies/config/imgui"
    include "dependencies/config/imgui-node-editor"
    include "dependencies/config/imgui-file-dialog"
    include "dependencies/config/nvrhi"
    include "dependencies/config/catch2"
    include "dependencies/config/box2d"
    include "dependencies/config/tracy"
    -- spdlog is header-only, no build needed
group ""

-- Lumina Modules
group "lumina"
    include "lumina/core"
    include "lumina/ui"
    include "lumina/graphics"
    include "lumina/physics"
group ""

-- Examples
group "examples"
    include "examples"
group ""

-- Sandbox/Test Application
group "sandbox"
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
            "lumina/physics/src",
            "dependencies/glfw/include",
            "dependencies/glm",
            "dependencies/imgui",
            "dependencies/imgui/backends",
            "dependencies/imgui-node-editor",
            "dependencies/imgui-file-dialog",
            "dependencies/box2d/include",
            "dependencies/spdlog/include",
            "dependencies/nvrhi/include",
            "dependencies/nvrhi/thirdparty/DirectX-Headers/include",
            "dependencies/nvrhi/thirdparty/Vulkan-Headers/include",
        }

        links {
            "core",
            "ui",
            "graphics",
            "physics",
            "box2d",
            "glfw",
            "imgui",
            "imgui-node-editor",
            "imgui-file-dialog",
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

-- Tests
group "tests"
    include "tests"
group ""
