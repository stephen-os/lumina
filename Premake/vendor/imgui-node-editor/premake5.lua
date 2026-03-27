-- imgui-node-editor dependency configuration
-- Builds imgui-node-editor as a static library

project "imgui-node-editor"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{wks.location}/Lumina/vendor/imgui-node-editor/crude_json.cpp",
        "%{wks.location}/Lumina/vendor/imgui-node-editor/crude_json.h",
        "%{wks.location}/Lumina/vendor/imgui-node-editor/imgui_bezier_math.h",
        "%{wks.location}/Lumina/vendor/imgui-node-editor/imgui_bezier_math.inl",
        "%{wks.location}/Lumina/vendor/imgui-node-editor/imgui_canvas.cpp",
        "%{wks.location}/Lumina/vendor/imgui-node-editor/imgui_canvas.h",
        "%{wks.location}/Lumina/vendor/imgui-node-editor/imgui_extra_math.h",
        "%{wks.location}/Lumina/vendor/imgui-node-editor/imgui_extra_math.inl",
        "%{wks.location}/Lumina/vendor/imgui-node-editor/imgui_node_editor.cpp",
        "%{wks.location}/Lumina/vendor/imgui-node-editor/imgui_node_editor.h",
        "%{wks.location}/Lumina/vendor/imgui-node-editor/imgui_node_editor_api.cpp",
        "%{wks.location}/Lumina/vendor/imgui-node-editor/imgui_node_editor_internal.h",
        "%{wks.location}/Lumina/vendor/imgui-node-editor/imgui_node_editor_internal.inl",
    }

    includedirs {
        "%{wks.location}/Lumina/vendor/imgui-node-editor",
        "%{wks.location}/Lumina/vendor/imgui",
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
