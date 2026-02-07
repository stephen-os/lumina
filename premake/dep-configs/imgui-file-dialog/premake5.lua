-- ImGuiFileDialog dependency configuration
-- Builds ImGuiFileDialog as a static library

project "imgui-file-dialog"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{wks.location}/dependencies/ImGuiFileDialog/ImGuiFileDialog.cpp",
        "%{wks.location}/dependencies/ImGuiFileDialog/ImGuiFileDialog.h",
        "%{wks.location}/dependencies/ImGuiFileDialog/ImGuiFileDialogConfig.h",
    }

    includedirs {
        "%{wks.location}/dependencies/ImGuiFileDialog",
        "%{wks.location}/dependencies/imgui",
    }

    defines {
        "IMGUI_DEFINE_MATH_OPERATORS"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "speed"

    filter "configurations:Dist"
        runtime "Release"
        optimize "speed"
        symbols "off"
