-- Lumina Examples
-- Auto-discovers and builds all example projects (supports nested folders)

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
            "%{wks.location}/Lumina/src",
            "%{wks.location}/Lumina/vendor/glfw/include",
            "%{wks.location}/Lumina/vendor/glm",
            "%{wks.location}/Lumina/vendor/imgui",
            "%{wks.location}/Lumina/vendor/imgui/backends",
            "%{wks.location}/Lumina/vendor/imgui-node-editor",
            "%{wks.location}/Lumina/vendor/imgui-file-dialog",
            "%{wks.location}/Lumina/vendor/spdlog/include",
            "%{wks.location}/Lumina/vendor/nvrhi/include",
            "%{wks.location}/Lumina/vendor/nvrhi/thirdparty/DirectX-Headers/include",
            "%{wks.location}/Lumina/vendor/nvrhi/thirdparty/Vulkan-Headers/include",
            "%{wks.location}/Lumina/vendor/box2d/include",
        }

        links {
            "Lumina",
            "glfw",
            "imgui",
            "imgui-node-editor",
            "imgui-file-dialog",
            "nvrhi",
            "nvrhi-d3d12",
            "nvrhi-vk",
            "box2d",
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

-- Recursively find all directories containing main.cpp
-- Returns table of { path = "full/path", group = "solution/folder" }
local function find_examples(base_dir, relative_group, depth)
    depth = depth or 0
    relative_group = relative_group or ""
    if depth > 2 then return {} end  -- Limit recursion depth

    local results = {}
    local dirs = os.matchdirs(base_dir .. "/*")

    for _, dir_path in ipairs(dirs) do
        local dir_name = path.getname(dir_path)
        local main_file = dir_path .. "/main.cpp"

        if os.isfile(main_file) then
            -- Found an example
            table.insert(results, {
                path = dir_path,
                group = relative_group
            })
        else
            -- Recurse into subdirectory, building the group path
            local sub_group = relative_group == "" and dir_name or (relative_group .. "/" .. dir_name)
            local sub_results = find_examples(dir_path, sub_group, depth + 1)
            for _, sub_entry in ipairs(sub_results) do
                table.insert(results, sub_entry)
            end
        end
    end

    return results
end

-- Auto-discover examples
local examples_dir = path.getdirectory(_SCRIPT)
local examples = find_examples(examples_dir)

-- Group examples by their solution folder
local current_group = nil
for _, example in ipairs(examples) do
    -- Set solution folder group (creates nested folders in VS)
    local target_group = example.group == "" and "examples" or ("examples/" .. example.group)
    if current_group ~= target_group then
        group(target_group)
        current_group = target_group
    end

    local example_name = path.getname(example.path)
    create_example(example_name, example.path)
end

-- Reset group
group("")
