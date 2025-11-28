project "Lumina"
   kind "StaticLib"
   language "C++"
   cppdialect "C++23"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   -- Enable multi-core compilation
   flags { "MultiProcessorCompile" }

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "src",

      "../Dependencies/imgui",
      "../Dependencies/glfw/include",
      "../Dependencies/glm",
      "../Dependencies/stb_image",
      "../Dependencies/glad/include",
      "../Dependencies/tinygltf",
      "../Dependencies/imguifd",
      "../Dependencies/spdlog/include",
      "../Dependencies/box2d/include",
      "../Dependencies/imgui-node-editor",
      "../Dependencies/imgui-node-editor/external/DXSDK/include"
   }

   links
   {
       "ImGui",
       "GLFW",
       "Glad",
       "TinyGLTF",
       "ImGuiFD",
       "Box2D",
       "ImGuiNodeEditor",

       "opengl32.lib"
   }

   defines
   {
      "GLFW_INCLUDE_NONE",
      "GLEW_STATIC",
      "IMGUI_DEFINE_MATH_OPERATORS"
   }

   buildoptions { "/utf-8" }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

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