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
        "%{wks.location}/Lumina/vendor/imgui/imgui.cpp",
        "%{wks.location}/Lumina/vendor/imgui/imgui.h",
        "%{wks.location}/Lumina/vendor/imgui/imgui_demo.cpp",
        "%{wks.location}/Lumina/vendor/imgui/imgui_draw.cpp",
        "%{wks.location}/Lumina/vendor/imgui/imgui_internal.h",
        "%{wks.location}/Lumina/vendor/imgui/imgui_tables.cpp",
        "%{wks.location}/Lumina/vendor/imgui/imgui_widgets.cpp",
        "%{wks.location}/Lumina/vendor/imgui/imstb_rectpack.h",
        "%{wks.location}/Lumina/vendor/imgui/imstb_textedit.h",
        "%{wks.location}/Lumina/vendor/imgui/imstb_truetype.h",

        -- C++ stdlib helpers
        "%{wks.location}/Lumina/vendor/imgui/misc/cpp/imgui_stdlib.cpp",
        "%{wks.location}/Lumina/vendor/imgui/misc/cpp/imgui_stdlib.h",

        -- GLFW backend (for input handling)
        "%{wks.location}/Lumina/vendor/imgui/backends/imgui_impl_glfw.cpp",
        "%{wks.location}/Lumina/vendor/imgui/backends/imgui_impl_glfw.h",

        -- TODO: Add NVRHI backend when available
        -- "%{wks.location}/Lumina/vendor/imgui/backends/imgui_impl_vulkan.cpp",
        -- "%{wks.location}/Lumina/vendor/imgui/backends/imgui_impl_vulkan.h",
    }

    includedirs {
        "%{wks.location}/Lumina/vendor/imgui",
        "%{wks.location}/Lumina/vendor/imgui/backends",
        "%{wks.location}/Lumina/vendor/glfw/include",
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
