project "tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"
    staticruntime "Off"

    targetdir ("%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/tests")
    objdir ("%{wks.location}/bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/tests")

    files
    {
        "**.h",
        "**.cpp"
    }

    includedirs
    {
        "%{wks.location}/dependencies/catch2/src",
        "%{wks.location}/lumina/core/src",
        "%{wks.location}/lumina/graphics/src",
        "%{wks.location}/dependencies/glfw/include",
        "%{wks.location}/dependencies/glm",
        "%{wks.location}/dependencies/imgui",
        "%{wks.location}/dependencies/spdlog/include",
        "%{wks.location}/dependencies/nvrhi/include",
        "%{wks.location}/dependencies/nvrhi/thirdparty/DirectX-Headers/include",
        "%{wks.location}/dependencies/nvrhi/thirdparty/Vulkan-Headers/include",
        "%{wks.location}/dependencies/stb_truetype",
        "%{wks.location}/dependencies/stb_image",
    }

    links
    {
        "core",
        "graphics",
        "catch2",
        "glfw",
        "nvrhi",
        "nvrhi-d3d12",
        "nvrhi-vk",
    }

    defines
    {
        "GLFW_INCLUDE_NONE",
        "NVRHI_WITH_DX12=1",
        "NVRHI_WITH_VULKAN=1",
        "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1",
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "LUMINA_PLATFORM_WINDOWS",
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX",
            "VK_USE_PLATFORM_WIN32_KHR",
        }
        libdirs { "$(VULKAN_SDK)/Lib" }
        links {
            "d3d12",
            "dxgi",
            "dxguid",
            "vulkan-1",
        }
        buildoptions { "/utf-8" }

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
        optimize "Full"
        symbols "Off"
