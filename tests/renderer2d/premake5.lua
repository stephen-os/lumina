project "TestRenderer2D"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   flags { "MultiProcessorCompile" }

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "%{wks.location}/lumina/src",

      "%{wks.location}/dependencies/imgui",
      "%{wks.location}/dependencies/glew/include",
      "%{wks.location}/dependencies/glfw/include",
      "%{wks.location}/dependencies/glm",
      "%{wks.location}/dependencies/glad/include",
      "%{wks.location}/dependencies/tinygltf",
      "%{wks.location}/dependencies/imguifd",
      "%{wks.location}/dependencies/spdlog/include"
   }

   links
   {
      "Lumina",
   }

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
      optimize "Off"

   filter "configurations:Release"
      defines { "LUMINA_RELEASE" }
      runtime "Release"
      optimize "Speed"
      symbols "On"

   filter "configurations:Dist"
      kind "WindowedApp"
      defines { "LUMINA_DIST" }
      runtime "Release"
      optimize "Speed"
      symbols "Off"