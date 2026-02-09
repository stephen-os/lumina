project "tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"
    staticruntime "Off"

    targetdir ("%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/tests")
    objdir ("%{wks.location}/bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/tests")

    files
    {
        "**.h",
        "**.cpp"
    }

    includedirs
    {
        "%{wks.location}/dependencies/catch2/src",
        "%{wks.location}/lumina/core/src",
        "%{wks.location}/dependencies/glfw/include",
        "%{wks.location}/dependencies/glm",
        "%{wks.location}/dependencies/imgui",
        "%{wks.location}/dependencies/spdlog/include",
        "%{wks.location}/dependencies/nvrhi/include",
    }

    links
    {
        "core",
        "catch2"
    }

    defines
    {
        "GLFW_INCLUDE_NONE"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "LUMINA_PLATFORM_WINDOWS" }
        buildoptions { "/utf-8" }

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
        optimize "Full"
        symbols "Off"
