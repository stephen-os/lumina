-- NVRHI Premake5 Build Configuration
-- Based on CMakeLists.txt from NVIDIA NVRHI

-- Configuration options (can be overridden before including this file)
NVRHI_WITH_VALIDATION = NVRHI_WITH_VALIDATION == nil and true or NVRHI_WITH_VALIDATION
NVRHI_WITH_VULKAN = NVRHI_WITH_VULKAN == nil and true or NVRHI_WITH_VULKAN
NVRHI_WITH_DX12 = NVRHI_WITH_DX12 == nil and true or NVRHI_WITH_DX12
NVRHI_WITH_DX11 = NVRHI_WITH_DX11 == nil and false or NVRHI_WITH_DX11  -- Disabled by default

local nvrhi_root = path.getdirectory(_SCRIPT)

-- Helper to get absolute paths
local function nvrhi_path(p)
    return path.join(nvrhi_root, p)
end

-- Store paths for external use
NVRHI_INCLUDE_DIR = nvrhi_path("include")
NVRHI_VULKAN_HEADERS_DIR = nvrhi_path("thirdparty/Vulkan-Headers/include")
NVRHI_DX_HEADERS_DIR = nvrhi_path("thirdparty/DirectX-Headers/include")

-------------------------------------------------------------------------------
-- NVRHI Core Library
-------------------------------------------------------------------------------
project "NVRHI"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        -- Common headers
        nvrhi_path("include/nvrhi/nvrhi.h"),
        nvrhi_path("include/nvrhi/nvrhiHLSL.h"),
        nvrhi_path("include/nvrhi/utils.h"),
        nvrhi_path("include/nvrhi/common/containers.h"),
        nvrhi_path("include/nvrhi/common/misc.h"),
        nvrhi_path("include/nvrhi/common/resource.h"),
        nvrhi_path("include/nvrhi/common/aftermath.h"),
        -- Common sources
        nvrhi_path("src/common/format-info.cpp"),
        nvrhi_path("src/common/misc.cpp"),
        nvrhi_path("src/common/state-tracking.cpp"),
        nvrhi_path("src/common/state-tracking.h"),
        nvrhi_path("src/common/utils.cpp"),
        nvrhi_path("src/common/aftermath.cpp")
    }

    -- Validation layer (always included when enabled)
    if NVRHI_WITH_VALIDATION then
        files {
            nvrhi_path("include/nvrhi/validation.h"),
            nvrhi_path("src/validation/validation-commandlist.cpp"),
            nvrhi_path("src/validation/validation-device.cpp"),
            nvrhi_path("src/validation/validation-backend.h")
        }
    end

    includedirs {
        nvrhi_path("include")
    }

    defines {
        "NVRHI_WITH_AFTERMATH=0"
    }

    filter "system:windows"
        systemversion "latest"
        files {
            nvrhi_path("tools/nvrhi.natvis")
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        runtime "Release"
        optimize "On"

    filter {}

-------------------------------------------------------------------------------
-- NVRHI D3D12 Backend (Windows only)
-------------------------------------------------------------------------------
if NVRHI_WITH_DX12 then
project "NVRHI_D3D12"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        nvrhi_path("include/nvrhi/d3d12.h"),
        nvrhi_path("src/common/dxgi-format.h"),
        nvrhi_path("src/common/dxgi-format.cpp"),
        nvrhi_path("src/common/versioning.h"),
        nvrhi_path("src/d3d12/d3d12-buffer.cpp"),
        nvrhi_path("src/d3d12/d3d12-commandlist.cpp"),
        nvrhi_path("src/d3d12/d3d12-compute.cpp"),
        nvrhi_path("src/d3d12/d3d12-constants.cpp"),
        nvrhi_path("src/d3d12/d3d12-backend.h"),
        nvrhi_path("src/d3d12/d3d12-descriptor-heap.cpp"),
        nvrhi_path("src/d3d12/d3d12-device.cpp"),
        nvrhi_path("src/d3d12/d3d12-graphics.cpp"),
        nvrhi_path("src/d3d12/d3d12-meshlets.cpp"),
        nvrhi_path("src/d3d12/d3d12-queries.cpp"),
        nvrhi_path("src/d3d12/d3d12-raytracing.cpp"),
        nvrhi_path("src/d3d12/d3d12-resource-bindings.cpp"),
        nvrhi_path("src/d3d12/d3d12-shader.cpp"),
        nvrhi_path("src/d3d12/d3d12-state-tracking.cpp"),
        nvrhi_path("src/d3d12/d3d12-texture.cpp"),
        nvrhi_path("src/d3d12/d3d12-upload.cpp")
    }

    includedirs {
        nvrhi_path("include"),
        nvrhi_path("thirdparty/DirectX-Headers/include")
    }

    -- WSL stubs only needed on non-Windows platforms
    filter "system:not windows"
        includedirs {
            nvrhi_path("thirdparty/DirectX-Headers/include/wsl/stubs")
        }
    filter {}

    links {
        "d3d12",
        "dxgi",
        "dxguid"
    }

    defines {
        "NVRHI_WITH_AFTERMATH=0",
        "NVRHI_D3D12_WITH_NVAPI=0"
    }

    filter "system:windows"
        systemversion "latest"

    -- Remove from non-Windows builds
    filter "system:not windows"
        kind "None"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        runtime "Release"
        optimize "On"

    filter {}
end

-------------------------------------------------------------------------------
-- NVRHI D3D11 Backend (Windows only, disabled by default)
-------------------------------------------------------------------------------
if NVRHI_WITH_DX11 then
project "NVRHI_D3D11"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        nvrhi_path("include/nvrhi/d3d11.h"),
        nvrhi_path("src/common/dxgi-format.h"),
        nvrhi_path("src/common/dxgi-format.cpp"),
        nvrhi_path("src/d3d11/d3d11-buffer.cpp"),
        nvrhi_path("src/d3d11/d3d11-commandlist.cpp"),
        nvrhi_path("src/d3d11/d3d11-compute.cpp"),
        nvrhi_path("src/d3d11/d3d11-constants.cpp"),
        nvrhi_path("src/d3d11/d3d11-backend.h"),
        nvrhi_path("src/d3d11/d3d11-device.cpp"),
        nvrhi_path("src/d3d11/d3d11-graphics.cpp"),
        nvrhi_path("src/d3d11/d3d11-queries.cpp"),
        nvrhi_path("src/d3d11/d3d11-resource-bindings.cpp"),
        nvrhi_path("src/d3d11/d3d11-shader.cpp"),
        nvrhi_path("src/d3d11/d3d11-texture.cpp")
    }

    includedirs {
        nvrhi_path("include")
    }

    links {
        "d3d11",
        "dxguid"
    }

    defines {
        "NVRHI_WITH_AFTERMATH=0",
        "NVRHI_D3D11_WITH_NVAPI=0"
    }

    filter "system:windows"
        systemversion "latest"

    filter "system:not windows"
        kind "None"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        runtime "Release"
        optimize "On"

    filter {}
end

-------------------------------------------------------------------------------
-- NVRHI Vulkan Backend
-------------------------------------------------------------------------------
if NVRHI_WITH_VULKAN then
project "NVRHI_VK"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        nvrhi_path("include/nvrhi/vulkan.h"),
        nvrhi_path("src/common/versioning.h"),
        nvrhi_path("src/vulkan/vulkan-allocator.cpp"),
        nvrhi_path("src/vulkan/vulkan-buffer.cpp"),
        nvrhi_path("src/vulkan/vulkan-commandlist.cpp"),
        nvrhi_path("src/vulkan/vulkan-compute.cpp"),
        nvrhi_path("src/vulkan/vulkan-constants.cpp"),
        nvrhi_path("src/vulkan/vulkan-device.cpp"),
        nvrhi_path("src/vulkan/vulkan-graphics.cpp"),
        nvrhi_path("src/vulkan/vulkan-meshlets.cpp"),
        nvrhi_path("src/vulkan/vulkan-queries.cpp"),
        nvrhi_path("src/vulkan/vulkan-queue.cpp"),
        nvrhi_path("src/vulkan/vulkan-raytracing.cpp"),
        nvrhi_path("src/vulkan/vulkan-resource-bindings.cpp"),
        nvrhi_path("src/vulkan/vulkan-shader.cpp"),
        nvrhi_path("src/vulkan/vulkan-staging-texture.cpp"),
        nvrhi_path("src/vulkan/vulkan-state-tracking.cpp"),
        nvrhi_path("src/vulkan/vulkan-texture.cpp"),
        nvrhi_path("src/vulkan/vulkan-upload.cpp"),
        nvrhi_path("src/vulkan/vulkan-backend.h")
    }

    includedirs {
        nvrhi_path("include"),
        nvrhi_path("thirdparty/Vulkan-Headers/include")
    }

    defines {
        "NVRHI_WITH_AFTERMATH=0"
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "VK_USE_PLATFORM_WIN32_KHR",
            "NOMINMAX"
        }

    filter "system:linux"
        defines {
            "VK_USE_PLATFORM_XLIB_KHR"
        }

    filter "system:macosx"
        defines {
            "VK_USE_PLATFORM_MACOS_MVK"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        runtime "Release"
        optimize "On"

    filter {}
end

-------------------------------------------------------------------------------
-- NVRHIConfig: Helper table for linking NVRHI to other projects
-- Usage: In your project, after including this file:
--   includedirs { NVRHI_INCLUDE_DIR, NVRHI_VULKAN_HEADERS_DIR }
--   links { "NVRHI", "NVRHI_VK", "NVRHI_D3D12" }
-------------------------------------------------------------------------------
NVRHIConfig = {
    includedirs = function()
        local dirs = { NVRHI_INCLUDE_DIR }
        if NVRHI_WITH_VULKAN then
            table.insert(dirs, NVRHI_VULKAN_HEADERS_DIR)
        end
        -- DirectX headers only needed on non-Windows (Windows uses SDK)
        return dirs
    end,

    links = function()
        local libs = { "NVRHI" }
        if NVRHI_WITH_VULKAN then
            table.insert(libs, "NVRHI_VK")
        end
        if NVRHI_WITH_DX12 then
            table.insert(libs, "NVRHI_D3D12")
        end
        if NVRHI_WITH_DX11 then
            table.insert(libs, "NVRHI_D3D11")
        end
        return libs
    end,

    defines = function()
        local defs = {}
        if NVRHI_WITH_VULKAN then
            table.insert(defs, "NVRHI_WITH_VULKAN=1")
        end
        if NVRHI_WITH_DX12 then
            table.insert(defs, "NVRHI_WITH_DX12=1")
        end
        if NVRHI_WITH_DX11 then
            table.insert(defs, "NVRHI_WITH_DX11=1")
        end
        return defs
    end
}
