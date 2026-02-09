project "catch2"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"

    targetdir ("%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/catch2")
    objdir ("%{wks.location}/bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/catch2")

    files
    {
        "%{wks.location}/dependencies/catch2/src/catch2/**.cpp",
        "%{wks.location}/dependencies/catch2/src/catch2/**.hpp"
    }

    removefiles
    {
        "%{wks.location}/dependencies/catch2/src/catch2/internal/catch_main.cpp"
    }

    includedirs
    {
        "%{wks.location}/dependencies/catch2/src"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        runtime "Release"
        optimize "Full"
