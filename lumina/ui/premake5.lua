-- ui premake5.lua
-- This file defines the UI static library

project "ui"
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

        -- Lumina core
        "%{wks.location}/lumina/core/src",

        -- Dependencies
        "%{wks.location}/dependencies/imgui",
        "%{wks.location}/dependencies/imgui/backends",
        "%{wks.location}/dependencies/glm",
        "%{wks.location}/dependencies/spdlog/include",
    }

    links {
        "core",
        "imgui",
    }

    defines {
        "IMGUI_DEFINE_MATH_OPERATORS",
    }

    flags { "MultiProcessorCompile" }
    buildoptions { "/utf-8" }

    filter "system:windows"
        systemversion "latest"
        defines {
            "LUMINA_PLATFORM_WINDOWS",
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
