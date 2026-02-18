-- ImGui dependency configuration
-- Builds ImGui as a static library with GLFW backend

project "imgui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{wks.location}/dependencies/imgui/imgui.cpp",
        "%{wks.location}/dependencies/imgui/imgui.h",
        "%{wks.location}/dependencies/imgui/imgui_demo.cpp",
        "%{wks.location}/dependencies/imgui/imgui_draw.cpp",
        "%{wks.location}/dependencies/imgui/imgui_internal.h",
        "%{wks.location}/dependencies/imgui/imgui_tables.cpp",
        "%{wks.location}/dependencies/imgui/imgui_widgets.cpp",
        "%{wks.location}/dependencies/imgui/imstb_rectpack.h",
        "%{wks.location}/dependencies/imgui/imstb_textedit.h",
        "%{wks.location}/dependencies/imgui/imstb_truetype.h",

        -- C++ stdlib helpers
        "%{wks.location}/dependencies/imgui/misc/cpp/imgui_stdlib.cpp",
        "%{wks.location}/dependencies/imgui/misc/cpp/imgui_stdlib.h",

        -- GLFW backend (for input handling)
        "%{wks.location}/dependencies/imgui/backends/imgui_impl_glfw.cpp",
        "%{wks.location}/dependencies/imgui/backends/imgui_impl_glfw.h",

        -- TODO: Add NVRHI backend when available
        -- "%{wks.location}/dependencies/imgui/backends/imgui_impl_vulkan.cpp",
        -- "%{wks.location}/dependencies/imgui/backends/imgui_impl_vulkan.h",
    }

    includedirs {
        "%{wks.location}/dependencies/imgui",
        "%{wks.location}/dependencies/imgui/backends",
        "%{wks.location}/dependencies/glfw/include",
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
