project "ImGuiNodeEditor"
	kind "StaticLib"
	language "C++"
	staticruntime "off"
	cppdialect "C++20"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs
	{
		".",
		"../imgui",

		"external/DXSDK/include",
		"external/DXSDK/src"
	}

	files
	{
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.inl"
	}

	excludes
	{
		"examples/**",
		"examples/*.h",
		"examples/*.hpp",
		"examples/*.cpp",
		"examples/*.inl",
		"docs/**",
		"test/**",
		"build/**"
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
		symbols "off"