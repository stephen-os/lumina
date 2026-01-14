-- Root config (when lumina is the main workspace)
LuminaConfigRoot =
{
    includedirs =
    {
        "%{wks.location}/lumina/src",

        "%{wks.location}/dependencies/glfw/include",
        "%{wks.location}/dependencies/glad/include",

        "%{wks.location}/dependencies/imgui",
        "%{wks.location}/dependencies/imguifd",
        "%{wks.location}/dependencies/imgui-node-editor",
        "%{wks.location}/dependencies/imgui-node-editor/external/DXSDK/include",

        "%{wks.location}/dependencies/glm",
        "%{wks.location}/dependencies/tinygltf",

        "%{wks.location}/dependencies/box2d/include",

        "%{wks.location}/dependencies/stb_image",
        "%{wks.location}/dependencies/spdlog/include",

        "%{wks.location}/dependencies/libuiohook/include"
    },

    links =
    {
        "GLFW",
        "Glad",

        "ImGui",
        "ImGuiFD",
        "ImGuiNodeEditor",

        "TinyGLTF",
        "Box2D",

        "LibUIOHook",

        "opengl32.lib"
    },

    defines =
    {
        "GLFW_INCLUDE_NONE",
        "GLEW_STATIC",
        "IMGUI_DEFINE_MATH_OPERATORS"
    }
}

-- Submodule config (when lumina is included as a submodule)
LuminaConfigSubmodule =
{
    includedirs =
    {
        "%{wks.location}/lumina/lumina/src",

        "%{wks.location}/lumina/dependencies/glfw/include",
        "%{wks.location}/lumina/dependencies/glad/include",

        "%{wks.location}/lumina/dependencies/imgui",
        "%{wks.location}/lumina/dependencies/imguifd",
        "%{wks.location}/lumina/dependencies/imgui-node-editor",
        "%{wks.location}/lumina/dependencies/imgui-node-editor/external/DXSDK/include",

        "%{wks.location}/lumina/dependencies/glm",
        "%{wks.location}/lumina/dependencies/tinygltf",

        "%{wks.location}/lumina/dependencies/box2d/include",

        "%{wks.location}/lumina/dependencies/stb_image",
        "%{wks.location}/lumina/dependencies/spdlog/include",

        "%{wks.location}/lumina/dependencies/libuiohook/include"
    },

    links =
    {
        "GLFW",
        "Glad",

        "ImGui",
        "ImGuiFD",
        "ImGuiNodeEditor",

        "TinyGLTF",
        "Box2D",

        "LibUIOHook",

        "opengl32.lib"
    },

    defines =
    {
        "GLFW_INCLUDE_NONE",
        "GLEW_STATIC",
        "IMGUI_DEFINE_MATH_OPERATORS"
    }
}

-- Select config based on LuminaAsSubmodule flag (set this to true before including this file)
LuminaConfig = LuminaAsSubmodule and LuminaConfigSubmodule or LuminaConfigRoot

ApplicationConfig = {}

ApplicationConfig.includedirs = {}
for _, path in ipairs(LuminaConfig.includedirs) do
    table.insert(ApplicationConfig.includedirs, path)
end

ApplicationConfig.links = { "Lumina" }
for _, lib in ipairs(LuminaConfig.links) do
    table.insert(ApplicationConfig.links, lib)
end

ApplicationConfig.defines = {}
for _, def in ipairs(LuminaConfig.defines) do
    table.insert(ApplicationConfig.defines, def)
end

project "Lumina"
   kind "StaticLib"
   language "C++"
   cppdialect "C++23"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   flags { "MultiProcessorCompile" }

   files { "src/**.h", "src/**.cpp" }

   includedirs(LuminaConfig.includedirs)

   links(LuminaConfig.links)

   defines(LuminaConfig.defines)

   buildoptions { "/utf-8" }

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "LUMINA_PLATFORM_WINDOWS" }

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
