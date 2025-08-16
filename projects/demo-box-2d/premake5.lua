project "DemoBox2D"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "../../Lumina/src",

      "../../Dependencies/imgui",
      "../../Dependencies/glew/include",
      "../../Dependencies/glfw/include",
      "../../Dependencies/glm",
      "../../Dependencies/glad/include",
      "../../Dependencies/tinygltf",
      "../../Dependencies/imguifd",
      "../../Dependencies/spdlog/include",
      "../../Dependencies/box2d/include"
   }

    links
    {
        "Lumina",

        "ImGui",
        "GLFW",
        "Glad",
        "TinyGLTF",
        "ImGuiFD",
        "Box2D",

        "opengl32.lib"
    }

    buildoptions { "/utf-8" }

   targetdir ("../../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../../bin-int/" .. outputdir .. "/%{prj.name}")

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
      kind "WindowedApp"
      defines { "LUMINA_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"