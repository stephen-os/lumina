-- Tracy profiler dependency configuration
-- Tracy requires compiling TracyClient.cpp once

project "tracy"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{wks.location}/dependencies/tracy/public/TracyClient.cpp"
    }

    includedirs {
        "%{wks.location}/dependencies/tracy/public"
    }

    defines {
        "TRACY_ENABLE",          -- Enable Tracy profiling
        "TRACY_ON_DEMAND",       -- Only connect when profiler is attached
        "TRACY_NO_SYSTEM_TRACING", -- Disable ETW (Event Tracing for Windows)
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "_CRT_SECURE_NO_WARNINGS"
        }
        links {
            "ws2_32",            -- Winsock for network communication
            "dbghelp",           -- For callstack capture
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        -- In Dist builds, Tracy is disabled (no TRACY_ENABLE)
        defines {
            "TRACY_ENABLE=0"
        }
        runtime "Release"
        optimize "On"
        symbols "Off"
