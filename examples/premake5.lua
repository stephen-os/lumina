-- Lumina Examples
-- Auto-discovers and builds all example projects

local function create_example(name, folder)
    project(name)
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++23"
        staticruntime "off"

        targetdir ("%{wks.location}/bin/" .. outputdir .. "/examples/%{prj.name}")
        objdir ("%{wks.location}/bin-int/" .. outputdir .. "/examples/%{prj.name}")

        files {
            folder .. "/**.h",
            folder .. "/**.cpp"
        }

        includedirs {
            "%{wks.location}/lumina/core/src",
            "%{wks.location}/lumina/ui/src",
            "%{wks.location}/lumina/graphics/src",
            "%{wks.location}/dependencies/glfw/include",
            "%{wks.location}/dependencies/glm",
            "%{wks.location}/dependencies/imgui",
            "%{wks.location}/dependencies/imgui/backends",
            "%{wks.location}/dependencies/imgui-node-editor",
            "%{wks.location}/dependencies/imgui-file-dialog",
            "%{wks.location}/dependencies/spdlog/include",
            "%{wks.location}/dependencies/nvrhi/include",
            "%{wks.location}/dependencies/nvrhi/thirdparty/DirectX-Headers/include",
            "%{wks.location}/dependencies/nvrhi/thirdparty/Vulkan-Headers/include",
        }

        links {
            "core",
            "ui",
            "graphics",
            "glfw",
            "imgui",
            "imgui-node-editor",
            "imgui-file-dialog",
            "nvrhi",
            "nvrhi-d3d12",
            "nvrhi-vk",
        }

        defines {
            "GLFW_INCLUDE_NONE",
            "IMGUI_DEFINE_MATH_OPERATORS",
            "NVRHI_WITH_DX12=1",
            "NVRHI_WITH_VULKAN=1",
            "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1",
        }

        buildoptions { "/utf-8" }

        filter "system:windows"
            systemversion "latest"
            defines {
                "LUMINA_PLATFORM_WINDOWS",
                "VK_USE_PLATFORM_WIN32_KHR",
            }
            libdirs { "$(VULKAN_SDK)/Lib" }
            links {
                "d3d12",
                "dxgi",
                "dxguid",
                "vulkan-1",
            }

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

        filter {}
end

-- Auto-discover examples by looking for folders with main.cpp
local examples_dir = path.getdirectory(_SCRIPT)
local examples = os.matchdirs(examples_dir .. "/*")

for _, example_path in ipairs(examples) do
    local example_name = path.getname(example_path)
    local main_file = example_path .. "/main.cpp"

    -- Skip if no main.cpp (e.g., common folder, premake5.lua)
    if os.isfile(main_file) then
        create_example(example_name, example_path)
    end
end
