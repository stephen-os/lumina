-- Box2D dependency configuration
-- Builds Box2D as a static library

project "box2d"
    kind "StaticLib"
    language "C"
    staticruntime "off"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{wks.location}/Lumina/vendor/box2d/src/**.h",
        "%{wks.location}/Lumina/vendor/box2d/src/**.c",
        "%{wks.location}/Lumina/vendor/box2d/include/**.h"
    }

    includedirs {
        "%{wks.location}/Lumina/vendor/box2d/include",
        "%{wks.location}/Lumina/vendor/box2d/src"
    }

    filter "system:windows"
        systemversion "latest"

        defines {
            "_CRT_SECURE_NO_WARNINGS"
        }

        buildoptions {
            "/std:c11",
            "/experimental:c11atomics"
        }

    filter "system:linux"
        pic "On"

        buildoptions {
            "-std=c11"
        }

        links {
            "m"
        }

    filter "system:macosx"
        buildoptions {
            "-std=c11"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        defines { "DEBUG" }

    filter "configurations:Release"
        runtime "Release"
        optimize "speed"
        defines { "NDEBUG" }

    filter "configurations:Dist"
        runtime "Release"
        optimize "speed"
        symbols "off"
        defines { "NDEBUG" }
