-- GLFW dependency configuration
-- Builds GLFW as a static library

project "glfw"
    kind "StaticLib"
    language "C"
    staticruntime "off"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{wks.location}/Lumina/vendor/glfw/include/GLFW/glfw3.h",
        "%{wks.location}/Lumina/vendor/glfw/include/GLFW/glfw3native.h",
        "%{wks.location}/Lumina/vendor/glfw/src/glfw_config.h",
        "%{wks.location}/Lumina/vendor/glfw/src/context.c",
        "%{wks.location}/Lumina/vendor/glfw/src/init.c",
        "%{wks.location}/Lumina/vendor/glfw/src/input.c",
        "%{wks.location}/Lumina/vendor/glfw/src/monitor.c",
        "%{wks.location}/Lumina/vendor/glfw/src/null_init.c",
        "%{wks.location}/Lumina/vendor/glfw/src/null_joystick.c",
        "%{wks.location}/Lumina/vendor/glfw/src/null_monitor.c",
        "%{wks.location}/Lumina/vendor/glfw/src/null_window.c",
        "%{wks.location}/Lumina/vendor/glfw/src/platform.c",
        "%{wks.location}/Lumina/vendor/glfw/src/vulkan.c",
        "%{wks.location}/Lumina/vendor/glfw/src/window.c"
    }

    filter "system:windows"
        systemversion "latest"

        files {
            "%{wks.location}/Lumina/vendor/glfw/src/win32_init.c",
            "%{wks.location}/Lumina/vendor/glfw/src/win32_joystick.c",
            "%{wks.location}/Lumina/vendor/glfw/src/win32_module.c",
            "%{wks.location}/Lumina/vendor/glfw/src/win32_monitor.c",
            "%{wks.location}/Lumina/vendor/glfw/src/win32_time.c",
            "%{wks.location}/Lumina/vendor/glfw/src/win32_thread.c",
            "%{wks.location}/Lumina/vendor/glfw/src/win32_window.c",
            "%{wks.location}/Lumina/vendor/glfw/src/wgl_context.c",
            "%{wks.location}/Lumina/vendor/glfw/src/egl_context.c",
            "%{wks.location}/Lumina/vendor/glfw/src/osmesa_context.c"
        }

        defines {
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "system:linux"
        pic "On"

        files {
            "%{wks.location}/Lumina/vendor/glfw/src/x11_init.c",
            "%{wks.location}/Lumina/vendor/glfw/src/x11_monitor.c",
            "%{wks.location}/Lumina/vendor/glfw/src/x11_window.c",
            "%{wks.location}/Lumina/vendor/glfw/src/xkb_unicode.c",
            "%{wks.location}/Lumina/vendor/glfw/src/posix_module.c",
            "%{wks.location}/Lumina/vendor/glfw/src/posix_time.c",
            "%{wks.location}/Lumina/vendor/glfw/src/posix_thread.c",
            "%{wks.location}/Lumina/vendor/glfw/src/posix_poll.c",
            "%{wks.location}/Lumina/vendor/glfw/src/glx_context.c",
            "%{wks.location}/Lumina/vendor/glfw/src/egl_context.c",
            "%{wks.location}/Lumina/vendor/glfw/src/osmesa_context.c",
            "%{wks.location}/Lumina/vendor/glfw/src/linux_joystick.c"
        }

        defines {
            "_GLFW_X11"
        }

    filter "system:macosx"
        pic "On"

        files {
            "%{wks.location}/Lumina/vendor/glfw/src/cocoa_init.m",
            "%{wks.location}/Lumina/vendor/glfw/src/cocoa_monitor.m",
            "%{wks.location}/Lumina/vendor/glfw/src/cocoa_window.m",
            "%{wks.location}/Lumina/vendor/glfw/src/cocoa_joystick.m",
            "%{wks.location}/Lumina/vendor/glfw/src/cocoa_time.c",
            "%{wks.location}/Lumina/vendor/glfw/src/nsgl_context.m",
            "%{wks.location}/Lumina/vendor/glfw/src/posix_thread.c",
            "%{wks.location}/Lumina/vendor/glfw/src/posix_module.c",
            "%{wks.location}/Lumina/vendor/glfw/src/egl_context.c",
            "%{wks.location}/Lumina/vendor/glfw/src/osmesa_context.c"
        }

        defines {
            "_GLFW_COCOA"
        }

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
