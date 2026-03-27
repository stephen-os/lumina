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
    include "Premake/vendor/glfw"
    include "Premake/vendor/imgui"
    include "Premake/vendor/imgui-node-editor"
    include "Premake/vendor/imgui-file-dialog"
    include "Premake/vendor/nvrhi"
    include "Premake/vendor/catch2"
    include "Premake/vendor/box2d"
    include "Premake/vendor/tracy"
    -- spdlog is header-only, no build needed
group ""

-- Lumina Engine
group "Lumina"
    include "Lumina"
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
            "Lumina/src",
            "Lumina/vendor/glfw/include",
            "Lumina/vendor/glm",
            "Lumina/vendor/imgui",
            "Lumina/vendor/imgui/backends",
            "Lumina/vendor/imgui-node-editor",
            "Lumina/vendor/imgui-file-dialog",
            "Lumina/vendor/box2d/include",
            "Lumina/vendor/spdlog/include",
            "Lumina/vendor/nvrhi/include",
            "Lumina/vendor/nvrhi/thirdparty/DirectX-Headers/include",
            "Lumina/vendor/nvrhi/thirdparty/Vulkan-Headers/include",
        }

        links {
            "Lumina",
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
