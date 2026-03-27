project "ImGuiFD"
	kind "StaticLib"
	language "C++"
    cppdialect "C++20"
    staticruntime "off"

	-- Enable multi-core compilation
	flags { "MultiProcessorCompile" }

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"ImGuiFileDialog.h",
		"ImGuiFileDialog.cpp",

		"ImGuiFileDialogConfig.h",

		"stb/stb_image.h",
		"stb/stb_image_resize2.h"
	}

	includedirs
	{
		"../imgui",
		"."
	}

	links { "ImGui" }

	defines 
    { 
        "USE_THUMBNAILS",
        "DONT_DEFINE_AGAIN__STB_IMAGE_IMPLEMENTATION"
    } 

 -- Platform-specific settings
    filter "system:windows"
        systemversion "latest"
        
    filter "system:linux"
        pic "On"
        systemversion "latest"
        
    filter "system:macosx"
        systemversion "latest"
        
    -- Configuration-specific settings
    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        defines { "DEBUG" }
        
    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        defines { "NDEBUG" }
        
    filter "configurations:Dist"
        runtime "Release"
        optimize "On"
        symbols "Off"
        defines { "NDEBUG", "DIST_BUILD" }
        
    -- Clear filters
    filter {}
