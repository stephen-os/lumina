project "LuminaApplication"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++23"
   staticruntime "off"
   
   files { "src/**.h", "src/**.cpp" }
   
   includedirs(ApplicationConfig.includedirs)
   
   links(ApplicationConfig.links)
   
   defines(ApplicationConfig.defines)
   
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
      kind "WindowedApp"
      defines { "LUMINA_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"